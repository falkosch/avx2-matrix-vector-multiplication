#include "avx2-variant.h"

#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

using namespace std;
using namespace matrixmultiplication::scalar;

namespace matrixmultiplication::avx2
{
    // value of elements added due to padding of the data sizes
    constexpr float PADDING_VALUE{-0.0F};

    // we have 8 single precision float elements in an AVX register
    constexpr size_t NUM_FLOATS_PER_AVX_REGISTER{sizeof(__m256) /
                                                 sizeof(float)};

    // we pad lengths by the count of elements in the AVX registers
    // so that we have it easier to handle the vector data in the
    // transformation
    auto padSize(const size_t size) noexcept
    {
        assert(size > 0);

        return (size - size_t{1}) / NUM_FLOATS_PER_AVX_REGISTER + size_t{1};
    }

    template <int i> auto broadcast(const __m256 & value) noexcept
    {
        return _mm256_permute_ps(value, _MM_SHUFFLE(i, i, i, i));
    }

    auto unpackLow(const __m256 & packed) noexcept
    {
        return _mm256_permute2f128_ps(packed, packed, 0);
    }

    auto unpackHigh(const __m256 & packed) noexcept
    {
        return _mm256_permute2f128_ps(packed, packed, 0b00010001);
    }

    auto multiplyAdd(const __m256 & partialColumn,
                     const __m256 & inputBroadcast,
                     const __m256 & partialResult) noexcept
    {
        // will be optimized to a real hardware FMA op if available
        return _mm256_add_ps(_mm256_mul_ps(partialColumn, inputBroadcast),
                             partialResult);
    }

    AVXVector::AVXVector(const size_t elements,
                         const float initialValue) noexcept
        : _elements{elements}, _packs(padSize(elements))
    {
        assert(elements > 0);

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
        auto pack = this->_packs.at(i / NUM_FLOATS_PER_AVX_REGISTER);
        return pack.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    SOAMatrix::SOAMatrix(const ScalarMatrix & original) noexcept
        : _rows{original.rows()}, _columns{original.columns()},
          _rowsPaddedSize{padSize(original.rows())},
          _packs(padSize(original.rows()) * original.columns())
    {
        assert(original.rows() > 0);
        assert(original.columns() > 0);

        for (auto && pack : this->_packs)
        {
            _mm256_store_ps(pack.data(), _mm256_set1_ps(PADDING_VALUE));
        }

        // the following code looks like a copy operation of the data of the
        // original AOSMatrix into this SOAMatrix however, it is the
        // conversion of the layout of the data from AOS to SOA by simply
        // transposing it in memory. thereby, the count of elements keeps
        // the same as does the unpadded size of the data and the data
        // itself.
        for (size_t r{0}; r < this->_rows; ++r)
        {
            for (size_t c{0}; c < this->_columns; ++c)
            {
                // see "float & at(size_t r, size_t c)" for details
                this->at(r, c) = original.at(r, c);
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
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        auto & pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }

    float SOAMatrix::at(const size_t r, const size_t c) const noexcept
    {
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->_rowsPaddedSize;
        auto pack = this->_packs.at(i);
        return pack.at(r % NUM_FLOATS_PER_AVX_REGISTER);
    }

    // multiplies a broadcast vector of one column element in the input
    // vector element-wise with the column vector of a matrix (that is the
    // row in the SOAMatrix). helps the SOAMatrix.avxTransform(const
    // AVXVector &) to split some code.
    struct TransformHelper
    {
        size_t packsPerColumn;
        vector<AVXPack>::const_iterator dataStart;
        vector<AVXPack>::iterator resultStart;
        vector<AVXPack>::iterator resultEnd;

        TransformHelper(const size_t rows,
                        const vector<AVXPack>::const_iterator dataStart,
                        const vector<AVXPack>::iterator resultStart) noexcept
            : packsPerColumn(padSize(rows)), dataStart(dataStart),
              resultStart(resultStart),
              resultEnd(resultStart + static_cast<int64_t>(padSize(rows)))
        {
            assert(rows > 0);
        }

        void operator()(const size_t column,
                        const __m256 & inputBroadcast) const noexcept
        {
            // Thanks to the SOA layout, each row in SOAMatrix contains AVX
            // fitting partial vectors of the elements of the corresponding
            // column "c" in the AOS matrix, so that we can directly load
            // the data into fitting AVX registers, multiply the elements
            // and add the result back into our intermediate result vector.
            auto rowIt = this->dataStart +
                         static_cast<int64_t>(column * this->packsPerColumn);
            for (auto resultIt = this->resultStart; resultIt != this->resultEnd;
                 resultIt++, rowIt++)
            {
                auto partialColumn = _mm256_load_ps(rowIt->data());
                auto partialResult = _mm256_load_ps(resultIt->data());
                auto intermediate =
                    multiplyAdd(partialColumn, inputBroadcast, partialResult);
                _mm256_store_ps(resultIt->data(), intermediate);
            }
        }
    };

    AVXVector transform(const SOAMatrix & matrix,
                        const AVXVector & inputVector) noexcept
    {
        assert(inputVector.size() == matrix.columns());

        // HERE is the MOST IMPORTANT code in this example.

        const auto rows = matrix.rows();
        const auto columns = matrix.columns();

        AVXVector result{rows, 0.0F};

        TransformHelper transformHelper{rows, matrix.packs().cbegin(),
                                        result.packs().begin()};

        // remember that we have the SOA layout, so we iterate on the
        // columns in the outer most loop. in the inner loop, we vertically
        // do the partial dot product step by step.
        size_t c{0};
        for (auto && inputPack : inputVector.packs())
        {
            // thanks to the padding we can safely load partials of the data
            // into one whole AVX register at once
            auto partialInput = _mm256_load_ps(inputPack.data());

            // we unroll the NUM_FLOATS_PER_AVX_REGISTER-times loop on the
            // partial input vector and broadcast each element for one
            // iteration in that loop

            auto partialInputLow = unpackLow(partialInput);
            auto partialInputHigh = unpackHigh(partialInput);

            if ((c + 0) < columns)
            {
                transformHelper(c + 0, broadcast<0>(partialInputLow));
            }
            if ((c + 1) < columns)
            {
                transformHelper(c + 1, broadcast<1>(partialInputLow));
            }
            if ((c + 2) < columns)
            {
                transformHelper(c + 2, broadcast<2>(partialInputLow));
            }
            if ((c + 3) < columns)
            {
                transformHelper(c + 3, broadcast<3>(partialInputLow));
            }

            if ((c + 4) < columns)
            {
                transformHelper(c + 4, broadcast<0>(partialInputHigh));
            }
            if ((c + 5) < columns)
            {
                transformHelper(c + 5, broadcast<1>(partialInputHigh));
            }
            if ((c + 6) < columns)
            {
                transformHelper(c + 6, broadcast<2>(partialInputHigh));
            }
            if ((c + 7) < columns)
            {
                transformHelper(c + 7, broadcast<3>(partialInputHigh));
            }

            c += NUM_FLOATS_PER_AVX_REGISTER;
        }

        return result;
    }
}
