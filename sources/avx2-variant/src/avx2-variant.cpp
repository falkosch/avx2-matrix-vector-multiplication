#include "avx2-variant.h"

#include <cassert>

using namespace std;
using namespace matrixmultiplication::scalar;

namespace matrixmultiplication
{
    namespace avx2
    {

        // value of elements added due to padding of the data sizes
#define PADDING_VALUE 0.0f

// we have 8 single precision float elements in an AVX register
#define NUM_FLOATS_PER_AVX_REGISTER (sizeof(__m256) / sizeof(float))

        // we pad lengths by the count of elements in the AVX registers
        // so that we have it easier to handle the vector data in the
        // transformation
        auto padSize(const size_t size)
        {
            assert(size > 0);

            return (size - size_t{1}) / NUM_FLOATS_PER_AVX_REGISTER + size_t{1};
        }

        template <int i> auto broadcast(const __m256 & value)
        {
            return _mm256_permute_ps(value, _MM_SHUFFLE(i, i, i, i));
        }

        auto unpackLow(const __m256 & packed)
        {
            return _mm256_permute2f128_ps(packed, packed, 0);
        }

        auto unpackHigh(const __m256 & packed)
        {
            return _mm256_permute2f128_ps(packed, packed, 0b00010001);
        }

        auto multiplyAdd(const __m256 & partialColumn,
                         const __m256 & inputBroadcast,
                         const __m256 & partialResult)
        {
            // will be optimized to a real hardware FMA op if available
            return _mm256_add_ps(_mm256_mul_ps(partialColumn, inputBroadcast),
                                 partialResult);
        }

        AVXPack::AVXPack(const __m256 & packed) : packed{packed}
        {
        }

        AVXVector::AVXVector(const size_t elements, const float initialValue)
            : elements{elements},
              packs(padSize(elements), AVXPack{_mm256_setzero_ps()})
        {
            assert(elements > 0);

            for (size_t i{0}; i < elements; ++i)
            {
                this->at(i) = initialValue;
            }
        }

        float & AVXVector::at(const size_t i)
        {
            return this->packs.at(i / NUM_FLOATS_PER_AVX_REGISTER)
                .components.at(i % NUM_FLOATS_PER_AVX_REGISTER);
        }

        float AVXVector::at(const size_t i) const
        {
            return this->packs.at(i / NUM_FLOATS_PER_AVX_REGISTER)
                .components.at(i % NUM_FLOATS_PER_AVX_REGISTER);
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
                            const vector<AVXPack>::iterator resultStart)
                : packsPerColumn(padSize(rows)), dataStart(dataStart),
                  resultStart(resultStart),
                  resultEnd(resultStart + static_cast<int64_t>(padSize(rows)))
            {
                assert(rows > 0);
            }

            void operator()(const size_t column,
                            const __m256 & inputBroadcast) const
            {
                // Thanks to the SOA layout, each row in SOAMatrix contains AVX
                // fitting partial vectors of the elements of the corresponding
                // column "c" in the AOS matrix, so that we can directly load
                // the data into fitting AVX registers, multiply the elements
                // and add the result back into our intermediate result vector.
                auto rowIt =
                    this->dataStart +
                    static_cast<int64_t>(column * this->packsPerColumn);
                for (auto resultIt = this->resultStart;
                     resultIt != this->resultEnd; resultIt++, rowIt++)
                {
                    auto partialColumn = (*rowIt).packed;
                    auto partialResult = (*resultIt).packed;
                    auto intermediate = multiplyAdd(
                        partialColumn, /* multiply by */ inputBroadcast,
                        /* and add */ partialResult);
                    (*resultIt).packed = intermediate;
                }
            }
        };

        SOAMatrix::SOAMatrix(const ScalarMatrix & original)
            : rows{original.rows}, columns{original.columns},
              rowsPaddedSize{padSize(rows)},
              packs(padSize(rows) * original.columns,
                    AVXPack{_mm256_set1_ps(PADDING_VALUE)})
        {
            assert(original.rows > 0);
            assert(original.columns > 0);

            // the following code looks like a copy operation of the data of the
            // original AOSMatrix into this SOAMatrix however, it is the
            // conversion of the layout of the data from AOS to SOA by simply
            // transposing it in memory. thereby, the count of elements keeps
            // the same as does the unpadded size of the data and the data
            // itself.
            for (size_t r{0}; r < this->rows; ++r)
            {
                for (size_t c{0}; c < this->columns; ++c)
                {
                    // see "float & at(size_t r, size_t c)" for details
                    this->at(r, c) = original.at(r, c);
                }
            }
        }

        float & SOAMatrix::at(const size_t r, const size_t c)
        {
            auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->rowsPaddedSize;
            return this->packs.at(i)
                .components[r % NUM_FLOATS_PER_AVX_REGISTER];
        }

        float SOAMatrix::at(const size_t r, const size_t c) const
        {
            auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->rowsPaddedSize;
            return this->packs.at(i)
                .components[r % NUM_FLOATS_PER_AVX_REGISTER];
        }

        // implements the scalar version of the matrix-vector multiplication as
        // template code
        const AVXVector SOAMatrix::avxTransform(
            const AVXVector & inputVector) const
        {
            assert(inputVector.elements == this->columns);

            // HERE is the MOST IMPORTANT code in this example.

            AVXVector result{this->rows, 0.0f};
            TransformHelper transformHelper{this->rows, this->packs.cbegin(),
                                            result.packs.begin()};

            // remember that we have the SOA layout, so we iterate on the
            // columns in the outer most loop. in the inner loop, we vertically
            // do the partial dot product step by step.
            size_t column{0};
            auto inputEnd = inputVector.packs.cend();
            for (auto inputStart = inputVector.packs.cbegin();
                 inputStart != inputEnd; inputStart++)
            {
                // thanks to the padding we can safely load partials of the data
                // into one whole AVX register at once
                auto partialInput = (*inputStart).packed;

                // we unroll the NUM_FLOATS_PER_AVX_REGISTER-times loop on the
                // partial input vector and broadcast each element for one
                // iteration in that loop

                auto partialInputLow = unpackLow(partialInput);
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<0>(partialInputLow));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<1>(partialInputLow));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<2>(partialInputLow));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<3>(partialInputLow));
                }
                ++column;

                auto partialInputHigh = unpackHigh(partialInput);
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<0>(partialInputHigh));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<1>(partialInputHigh));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<2>(partialInputHigh));
                }
                ++column;
                if (column < this->columns)
                {
                    transformHelper(column, broadcast<3>(partialInputHigh));
                }
                ++column;
            }

            return result;
        }

        // implements the scalar version of the matrix-vector multiplication as
        // template code
        const AVXVector SOAMatrix::scalarTransform(
            const AVXVector & inputVector) const
        {
            assert(inputVector.elements == this->columns);

            AVXVector result{this->rows, 0.0f};

            // remember that we have the SOA layout, so we iterate on the
            // columns in the outer most loop. in the inner loop we vertically
            // do the partial dot product step by step.
            for (size_t c{0}; c < this->columns; ++c)
            {
                for (size_t r{0}; r < this->rows; ++r)
                {
                    result.at(r) += this->at(r, c) * inputVector.at(c);
                }
            }

            return result;
        }

        // performs a RxC * Cx1->Rx1 matrix-vector multiplication
        const AVXVector SOAMatrix::transform(
            const AVXVector & inputVector) const
        {
            assert(inputVector.elements == this->columns);

            auto avxResult = this->avxTransform(inputVector);

#ifdef _DEBUG

            // verify correctness by comparing the scalar result against the
            // result of the AVX computation
            {
                auto scalarResult = this->scalarTransform(inputVector);
                assert(scalarResult.elements == avxResult.elements);
                for (size_t i{0}; i < scalarResult.elements; ++i)
                {
                    assert(scalarResult.at(i) == avxResult.at(i));
                }
            }

#endif

            return avxResult;
        }
    }
}
