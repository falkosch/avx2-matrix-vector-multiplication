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
    template <int i> const auto broadcast(const __m256 & value) noexcept
    {
        return _mm256_permute_ps(value, _MM_SHUFFLE(i, i, i, i));
    }

    const auto unpackLow(const __m256 & packed) noexcept
    {
        return _mm256_permute2f128_ps(packed, packed, 0);
    }

    const auto unpackHigh(const __m256 & packed) noexcept
    {
        return _mm256_permute2f128_ps(packed, packed, 0b00010001);
    }

    const auto multiplyAdd(const __m256 & partialColumn,
                           const __m256 & inputBroadcast,
                           const __m256 & partialResult) noexcept
    {
        // There is no need to enforce an FMA op here. An intelligent compiler
        // will optimize it to an FMA op if it is available for the target ARCH.
        return _mm256_add_ps(_mm256_mul_ps(partialColumn, inputBroadcast),
                             partialResult);
    }

    // Multiplies a broadcast vector of one column element in the input
    // vector element-wise with the column vector of a matrix (that is
    // represented by a row in the SOAMatrix). That helps to split the code of
    // the transform function.
    class TransformOperation
    {
        size_t packsPerColumn;
        vector<AVXPack>::const_iterator _dataStart;
        AVXVector & _resultVector;

      public:
        TransformOperation(const size_t rows,
                        const vector<AVXPack>::const_iterator dataStart,
                        AVXVector & resultVector) noexcept
            : packsPerColumn(padSize(rows)), _dataStart(dataStart),
              _resultVector(resultVector)
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
            auto rowIt = this->_dataStart +
                         static_cast<int64_t>(column * this->packsPerColumn);
            for (auto && resultPack : _resultVector.packs())
            {
                const auto partialColumn = _mm256_load_ps(rowIt->data());
                const auto partialResult = _mm256_load_ps(resultPack.data());
                const auto intermediate =
                    multiplyAdd(partialColumn, inputBroadcast, partialResult);
                _mm256_store_ps(resultPack.data(), intermediate);

                ++rowIt;
            }
        }
    };

    AVXVector transform(const SOAMatrix & matrix,
                        const AVXVector & inputVector) noexcept
    {
        assert(inputVector.size() == matrix.columns());

        // Here is the most important code in this example.

        const auto rows = matrix.rows();
        const auto columns = matrix.columns();

        AVXVector result{rows, 0.0F};

        TransformOperation transformOp{rows, matrix.packs().cbegin(), result};

        // Remember that we have the SOA layout, so we iterate on the
        // columns in the outer most loop. in the inner loop, we vertically
        // do the partial dot product step by step.
        size_t c{0};
        for (auto && inputPack : inputVector.packs())
        {
            // Thanks to the padding we can safely load partials of the data
            // into one whole AVX register at once.
            const auto partialInput = _mm256_load_ps(inputPack.data());

            // We unroll the NUM_FLOATS_PER_AVX_REGISTER-times loop on the
            // partial input vector and broadcast each element for one
            // iteration in that loop.
            const auto partialInputLow = unpackLow(partialInput);

            // Even the last AVXPack will always have at least one component, so
            // we do not need a check for that.
            transformOp(c + 0, broadcast<0>(partialInputLow));

            // However, all other components in the last pack could be part of
            // the padding.
            if ((c + 1) < columns)
            {
                transformOp(c + 1, broadcast<1>(partialInputLow));
            }
            if ((c + 2) < columns)
            {
                transformOp(c + 2, broadcast<2>(partialInputLow));
            }
            if ((c + 3) < columns)
            {
                transformOp(c + 3, broadcast<3>(partialInputLow));
            }

            if ((c + 4) < columns)
            {
                const auto partialInputHigh = unpackHigh(partialInput);

                transformOp(c + 4, broadcast<0>(partialInputHigh));

                if ((c + 5) < columns)
                {
                    transformOp(c + 5, broadcast<1>(partialInputHigh));
                }
                if ((c + 6) < columns)
                {
                    transformOp(c + 6, broadcast<2>(partialInputHigh));
                }
                if ((c + 7) < columns)
                {
                    transformOp(c + 7, broadcast<3>(partialInputHigh));
                }
            }

            c += NUM_FLOATS_PER_AVX_REGISTER;
        }

        return result;
    }
}