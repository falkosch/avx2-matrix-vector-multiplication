#include "avx2-variant-mt.h"

#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

using namespace std;

namespace matrixmultiplication::avx2
{
    auto broadcast(const __m256 & value, const int component) noexcept
    {
        const auto componentMask = _mm256_set1_epi32(component);
        return _mm256_permutevar8x32_ps(value, componentMask);
    }

    // Similar to the TransformOperation of the single-threaded avx2-variant,
    // this TransformOperation helps to split the code of the outer loop of
    // the matrix-vector transformation.
    class TransformOperation
    {
        size_t _rows;
        size_t packsPerColumn;
        vector<AVXPack>::const_iterator _dataStart;

      public:
        TransformOperation(
            const size_t rows,
            const vector<AVXPack>::const_iterator dataStart) noexcept
            : _rows(rows), packsPerColumn(padSize(rows)), _dataStart(dataStart)
        {
            assert(rows > 0);
        }

        AVXVector operator()(const size_t column,
                             const __m256 & inputBroadcast) const noexcept
        {
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

    AVXVector transformMultiThreaded(const SOAMatrix & matrix,
                                     const AVXVector & inputVector) noexcept
    {
        assert(inputVector.size() == matrix.columns());

        const auto rows = matrix.rows();
        const auto columns = matrix.columns();

        TransformOperation transformOp{rows, matrix.packs().cbegin()};
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
                for (auto i = int{0}; i < int{NUM_FLOATS_PER_AVX_REGISTER}; ++i)
                {
                    const auto inputBroadcast = broadcast(partialInput, i);
                    if ((c + i) < columns)
                    {
                        AVXVector product = transformOp(
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
