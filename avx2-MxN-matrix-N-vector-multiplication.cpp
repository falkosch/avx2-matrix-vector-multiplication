#include <iostream>
#include <array>
#include <vector>
#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

#define INITIAL_VALUE 1.0f

// AOS representation of a scalar matrix
struct ScalarMatrix
{
    std::size_t rows;
    std::size_t columns;

    std::vector<float> components;

    ScalarMatrix(const std::size_t rows, const std::size_t columns, const float initialValue = INITIAL_VALUE)
        : rows{ rows },
          columns{ columns },
          components(rows * columns, initialValue) {
        assert(rows > 0);
        assert(columns > 0);
    }

    float & at(const std::size_t r, const std::size_t c) {
        return this->components.at(r * this->columns + c);
    }

    auto at(const std::size_t r, const std::size_t c) const {
        return this->components.at(r * this->columns + c);
    }
};

#define PADDING_VALUE 0.0f // value of elements added due to padding of the data sizes

// we have 8 single precision float elements in an AVX register
#define NUM_FLOATS_PER_AVX_REGISTER (sizeof(__m256) / sizeof(float))

// we pad lengths by the count of elements in the AVX registers
// so that we have it easier to handle the vector data in the transformation
auto padSize(const std::size_t size) {
    assert(size > 0);

    return (size - std::size_t{ 1 }) / NUM_FLOATS_PER_AVX_REGISTER + std::size_t{ 1 };
}

template <int i>
auto broadcast(const __m256 & value) {
    return _mm256_permute_ps(value, _MM_SHUFFLE(i, i, i, i));
}

auto unpackLow(const __m256 & packed) {
    return _mm256_permute2f128_ps(packed, packed, 0);
}

auto unpackHigh(const __m256 & packed) {
    return _mm256_permute2f128_ps(packed, packed, 0b00010001);
}

auto multiplyAdd(const __m256 & partialColumn, const __m256 & inputBroadcast, const __m256 & partialResult) {
    // will be optimized to a real hardware FMA op if available
    return _mm256_add_ps(_mm256_mul_ps(partialColumn, inputBroadcast), partialResult);
}

union alignas(32) AVXPack
{
    __m256 packed;
    std::array<float, 8> components;

    AVXPack(const __m256 & packed)
        : packed{ packed } {
    }
};

struct AVXVector
{
    std::size_t elements;
    std::vector<AVXPack> packs;

    AVXVector(const std::size_t elements, const float initialValue = INITIAL_VALUE)
        : elements{ elements },
          packs(padSize(elements), AVXPack{ _mm256_setzero_ps() }) {
        assert(elements > 0);

        for (std::size_t i{ 0 }; i < elements; ++i) {
            this->at(i) = initialValue;
        }
    }

    float & at(const std::size_t i) {
        return this->packs.at(i / NUM_FLOATS_PER_AVX_REGISTER).components.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }

    auto at(const std::size_t i) const {
        return this->packs.at(i / NUM_FLOATS_PER_AVX_REGISTER).components.at(i % NUM_FLOATS_PER_AVX_REGISTER);
    }
};

// multiplies a broadcast vector of one column element in the input vector element-wise with the
// column vector of a matrix (that is the row in the SOAMatrix).
// helps the SOAMatrix.avxTransform(const AVXVector &) to split some code.
struct TransformHelper
{
    std::size_t packsPerColumn;
    std::vector<AVXPack>::const_iterator dataStart;
    std::vector<AVXPack>::iterator resultStart;
    std::vector<AVXPack>::iterator resultEnd;

    TransformHelper(const std::size_t rows, const std::vector<AVXPack>::const_iterator dataStart, const std::vector<AVXPack>::iterator resultStart)
        : packsPerColumn(padSize(rows)),
          dataStart(dataStart),
          resultStart(resultStart),
          resultEnd(resultStart + padSize(rows)) {
        assert(rows > 0);
    }

    void operator()(const std::size_t column, const __m256 & inputBroadcast) const {
        // Thanks to the SOA layout, each row in SOAMatrix contains AVX fitting partial vectors of
        // the elements of the corresponding column "c" in the AOS matrix, so that we can directly
        // load the data into fitting AVX registers, multiply the elements and add the result back
        // into our intermediate result vector.
        auto rowIt = this->dataStart + column * this->packsPerColumn;
        for (auto resultIt = this->resultStart;
             resultIt != this->resultEnd;
             resultIt++, rowIt++) {
            auto partialColumn = (*rowIt).packed;
            auto partialResult = (*resultIt).packed;
            auto intermediate = multiplyAdd(partialColumn, /* multiply by */ inputBroadcast, /* and add */ partialResult);
            (*resultIt).packed = intermediate;
        }
    }
};

struct SOAMatrix
{
    std::size_t rows;
    std::size_t columns;

    // count of how many AVXPacks make up a full column
    std::size_t rowsPaddedSize;

    std::vector<AVXPack> packs;

    SOAMatrix(const ScalarMatrix & original)
        : rows{ original.rows },
          columns{ original.columns },
          rowsPaddedSize{ padSize(rows) },
          packs(padSize(rows) * original.columns, AVXPack{ _mm256_set1_ps(PADDING_VALUE) }) {
        assert(original.rows > 0);
        assert(original.columns > 0);

        // the following code looks like a copy operation of the data of the original AOSMatrix into this SOAMatrix
        // however, it is the conversion of the layout of the data from AOS to SOA by simply transposing it in memory.
        // thereby, the count of elements keeps the same as does the unpadded size of the data and the data itself.
        for (std::size_t r{ 0 }; r < this->rows; ++r) {
            for (std::size_t c{ 0 }; c < this->columns; ++c) {
                // see "float & at(std::size_t r, std::size_t c)" for details
                this->at(r, c) = original.at(r, c);
            }
        }
    }

    float & at(const std::size_t r, const std::size_t c) {
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->rowsPaddedSize;
        return this->packs.at(i).components[r % NUM_FLOATS_PER_AVX_REGISTER];
    }

    auto at(const std::size_t r, const std::size_t c) const {
        auto i = r / NUM_FLOATS_PER_AVX_REGISTER + c * this->rowsPaddedSize;
        return this->packs.at(i).components[r % NUM_FLOATS_PER_AVX_REGISTER];
    }

    // implements the scalar version of the matrix-vector multiplication as template code
    const AVXVector avxTransform(const AVXVector & inputVector) const {
        assert(inputVector.elements == this->columns);

        // HERE is the MOST IMPORTANT code in this example.

        AVXVector result{ this->rows, 0.0f };
        TransformHelper transformHelper{ this->rows, this->packs.cbegin(), result.packs.begin() };

        // remember that we have the SOA layout, so we iterate on the columns in the outer most loop.
        // in the inner loop, we vertically do the partial dot product step by step.
        std::size_t column{ 0 };
        auto inputEnd = inputVector.packs.cend();
        for (auto inputStart = inputVector.packs.cbegin(); inputStart != inputEnd; inputStart++) {
            // thanks to the padding we can safely load partials of the data into one whole AVX register at once
            auto partialInput = (*inputStart).packed;

            // we unroll the NUM_FLOATS_PER_AVX_REGISTER-times loop on the partial input vector
            // and broadcast each element for one iteration in that loop

            auto partialInputLow = unpackLow(partialInput);
            if (column < this->columns) {
                transformHelper(column, broadcast<0>(partialInputLow));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<1>(partialInputLow));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<2>(partialInputLow));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<3>(partialInputLow));
            }
            ++column;

            auto partialInputHigh = unpackHigh(partialInput);
            if (column < this->columns) {
                transformHelper(column, broadcast<0>(partialInputHigh));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<1>(partialInputHigh));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<2>(partialInputHigh));
            }
            ++column;
            if (column < this->columns) {
                transformHelper(column, broadcast<3>(partialInputHigh));
            }
            ++column;
        }

        return result;
    }

    // implements the scalar version of the matrix-vector multiplication as template code
    const AVXVector scalarTransform(const AVXVector & inputVector) const {
        assert(inputVector.elements == this->columns);

        AVXVector result{ this->rows, 0.0f };

        // remember that we have the SOA layout, so we iterate on the columns in the outer most loop.
        // in the inner loop we vertically do the partial dot product step by step.
        for (std::size_t c{ 0 }; c < this->columns; ++c) {
            for (std::size_t r{ 0 }; r < this->rows; ++r) {
                result.at(r) += this->at(r, c) * inputVector.at(c);
            }
        }

        return result;
    }

    // performs a RxC * Cx1->Rx1 matrix-vector multiplication
    const AVXVector transform(const AVXVector & inputVector) const {
        assert(inputVector.elements == this->columns);

        auto avxResult = this->avxTransform(inputVector);

#ifdef _DEBUG

        // verify correctness by comparing the scalar result against the result of the AVX computation
        {
            auto scalarResult = this->scalarTransform(inputVector);
            assert(scalarResult.elements == avxResult.elements);
            for (std::size_t i{ 0 }; i < scalarResult.elements; ++i) {
                assert(scalarResult.at(i) == avxResult.at(i));
            }
        }

#endif

        return avxResult;
    }
};

int main() {
    // Be cautious with the sizes here. They determine how much memory is used when running the code.
    // 10000x10000 already uses around 220 MB memory.
    // On my i7 7700k it runs within 25s in debug mode.
    // In release mode it is less than 1s.
    std::size_t inputVectorSize{ 10000 };
    std::size_t outputVectorSize{ 10000 };

    ScalarMatrix aosTransform{ outputVectorSize, inputVectorSize };

    AVXVector inputVector{ inputVectorSize };

    // 1: we use a "structure of arrays" memory layout for the transform matrix
    // so that we can compute the transform in SIMD preferred way
    SOAMatrix soaTransform{ aosTransform };

    // 2: transform the input vector to the output vector, see SOAMatrix.transform()
    auto outputVector = soaTransform.transform(inputVector);

    // 3: verify the result. in this example code, the result should be the count of elements of
    // the input vector set on the first two elements of the output vector as all vector and
    // matrix elements are set to 1.0f.
    for (std::size_t i{ 0 }; i < outputVectorSize; ++i) {
        std::cout << outputVector.at(i) << " ";
    }

    return 0;
}