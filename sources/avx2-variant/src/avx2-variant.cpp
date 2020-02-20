#include "avx2-variant.h"

#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

using namespace std;

namespace matrixmultiplication::avx2
{
    // Value of elements added due to padding of the data sizes
    constexpr float PADDING_VALUE{-0.0F};

    // We have 8 single precision float elements in an AVX register
    constexpr size_t NUM_FLOATS_PER_AVX_REGISTER{sizeof(__m256) /
                                                 sizeof(float)};

    // We pad lengths by the count of elements in the AVX registers
    // so that we have it easier to handle the vector data in the
    // transformation.
    auto padSize(const size_t size) noexcept
    {
        assert(size > 0);

        return (size - size_t{1}) / NUM_FLOATS_PER_AVX_REGISTER + size_t{1};
    }

    auto broadcast(const __m256 & value, const int component) noexcept
    {
        const auto componentMask = _mm256_set1_epi32(component);
        return _mm256_permutevar8x32_ps(value, componentMask);
    }

    AVXVector::AVXVector() noexcept : _elements{0}, _packs(0)
    {
    }

    AVXVector::AVXVector(const size_t elements,
                         const float initialValue) noexcept
        : _elements{elements}, _packs(padSize(elements))
    {
        for (auto && pack : this->_packs)
        {
            _mm256_store_ps(pack.data(), _mm256_set1_ps(PADDING_VALUE));
        }

        for (size_t i{0}; i < this->_elements; ++i)
        {
            this->at(i) = initialValue;
        }
    }

    std::vector<AVXPack> & AVXVector::packs() noexcept
    {
        return this->_packs;
    }

    const std::vector<AVXPack> & AVXVector::packs() const noexcept
    {
        return this->_packs;
    }

    std::size_t AVXVector::size() const noexcept
    {
        return this->_elements;
    }

    float & AVXVector::at(const size_t i) noexcept
    {
        auto & pack = this->_packs.at(i / NUM_FLOATS_PER_AVX_REGISTER);
        return pack.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    float AVXVector::at(const size_t i) const noexcept
    {
        const auto pack = this->_packs.at(i / NUM_FLOATS_PER_AVX_REGISTER);
        return pack.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    SOAMatrix::SOAMatrix(const size_t rows, const size_t columns,
                         const float initialValue) noexcept
        : _rows{rows}, _columns{columns}, _rowsPaddedSize{padSize(rows)},
          _packs(padSize(rows) * columns)
    {
        assert(rows > 0);
        assert(columns > 0);

        for (auto && pack : this->_packs)
        {
            _mm256_store_ps(pack.data(), _mm256_set1_ps(PADDING_VALUE));
        }

        for (size_t r{0}; r < this->_rows; ++r)
        {
            for (size_t c{0}; c < this->_columns; ++c)
            {
                this->at(r, c) = initialValue;
            }
        }
    }

    std::vector<AVXPack> & SOAMatrix::packs() noexcept
    {
        return this->_packs;
    }

    const std::vector<AVXPack> & SOAMatrix::packs() const noexcept
    {
        return this->_packs;
    }

    std::size_t SOAMatrix::rows() const noexcept
    {
        return this->_rows;
    }

    std::size_t SOAMatrix::columns() const noexcept
    {
        return this->_columns;
    }

    float & SOAMatrix::at(const size_t r, const size_t c) noexcept
    {
        const auto i =
            r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        auto & pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }

    float SOAMatrix::at(const size_t r, const size_t c) const noexcept
    {
        const auto i =
            r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        const auto pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }

    // Multiplies a broadcast vector of one column element in the input
    // vector element-wise with the column vector of a matrix (that is
    // represented by a row in the SOAMatrix). That helps to split the code of
    // the transform function.
    class TransformHelper
    {
        size_t _rows;
        size_t packsPerColumn;
        vector<AVXPack>::const_iterator _dataStart;

      public:
        TransformHelper(
            const size_t rows,
            const vector<AVXPack>::const_iterator dataStart) noexcept
            : _rows(rows), packsPerColumn(padSize(rows)), _dataStart(dataStart)
        {
            assert(rows > 0);
        }

        AVXVector operator()(const size_t column,
                             const __m256 & inputBroadcast) const noexcept
        {
            // Thanks to the SOA layout, each row in SOAMatrix contains AVX
            // fitting partial vectors of the elements of the corresponding
            // column "c" in the AOS matrix, so that we can directly load
            // the data into fitting AVX registers, multiply the elements
            // and put the result back into an intermediate result vector.
            AVXVector resultVector{_rows, 0.0F};

            auto rowIt = this->_dataStart +
                         static_cast<int64_t>(column * this->packsPerColumn);

            for (auto && resultPack : resultVector.packs())
            {
                const auto partialColumn = _mm256_load_ps(rowIt->data());
                const auto intermediate =
                    _mm256_mul_ps(partialColumn, inputBroadcast);
                _mm256_store_ps(resultPack.data(), intermediate);

                ++rowIt;
            }

            return resultVector;
        }
    };

    void mergeIntermediateIntoResult(const AVXVector & intermediateVector,
                                     AVXVector & resultVector) noexcept
    {
        auto intermediateIt = intermediateVector.packs().cbegin();
        for (auto && resultPack : resultVector.packs())
        {
            const auto resultData = resultPack.data();
            const auto productPack = _mm256_load_ps(intermediateIt->data());
            const auto sumPack =
                _mm256_add_ps(_mm256_load_ps(resultData), productPack);
            _mm256_store_ps(resultData, sumPack);

            ++intermediateIt;
        }
    }

    AVXVector transform(const SOAMatrix & matrix,
                        const AVXVector & inputVector) noexcept
    {
        assert(inputVector.size() == matrix.columns());

        const auto rows = matrix.rows();
        const auto columns = matrix.columns();

        TransformHelper transformHelper{rows, matrix.packs().cbegin()};
        AVXVector resultVector{rows, 0.0F};

        // Remember that we have the SOA layout, so we iterate on the
        // columns in the outer most loop. in the inner loop, we vertically
        // do the partial dot product step by step.
        size_t c{0};
        for (auto && inputPack : inputVector.packs())
        {
            // Thanks to the padding we can safely load partials of the data
            // into one whole AVX register at once.
            const auto partialInput = _mm256_load_ps(inputPack.data());

            // We loop NUM_FLOATS_PER_AVX_REGISTER-times on the partial input
            // vector and broadcast each element for each iteration.

            // And we use openmp for each iteration in that loop when the
#pragma omp parallel
            {
                AVXVector intermediateVector{rows, 0.0F};

#pragma omp for nowait
                for (auto i = int{0}; i < int{8}; ++i)
                {
                    const auto inputBroadcast = broadcast(partialInput, i);
                    if ((c + i) < columns)
                    {
                        AVXVector product = transformHelper(
                            c + static_cast<size_t>(i), inputBroadcast);

                        mergeIntermediateIntoResult(product,
                                                    intermediateVector);
                    }
                }

#pragma omp critical
                {
                    mergeIntermediateIntoResult(intermediateVector,
                                                resultVector);
                }
            }

            c += NUM_FLOATS_PER_AVX_REGISTER;
        }

        return resultVector;
    }
}
