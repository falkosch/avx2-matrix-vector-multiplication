#include <iostream>
#include <vector>
#include <cassert>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

#include "AlignmentAllocator.h"

#define INITIAL_VALUE 1.0f
#define PADDING_VALUE 0.0f // value of elements added due to padding of the data sizes

// we have 8 single precision float elements in an AVX register
#define NUM_FLOATS_PER_AVX_REGISTER sizeof(__m256) / sizeof(float)
#define NUM_FLOATS_PER_HALF_AVX_REGISTER sizeof(__m128) / sizeof(float)

// we pad row lengths (count of columns) by the count of elements in the AVX registers
// so that we have it easier to handle the vector data in the transformation
const std::size_t padSize(const std::size_t size, const std::size_t padding = NUM_FLOATS_PER_AVX_REGISTER) {
    assert(size > 0);
    assert(padding > 0);

    return ((size - 1) / padding + 1) * padding;
}

// returns the count of AVX registers needed to fit all elements of a given size in bytes
const std::size_t avxRegistersNeeded(const std::size_t elements, const std::size_t elementSizeInBytes = sizeof(float)) {
    assert(elements > 0);
    assert(elementSizeInBytes > 0);

    return (elements * elementSizeInBytes - 1) / sizeof(__m256) + 1;
}

// returns the end ptr for the given counts of elements assuming the data layout is aligned to AVX register sizes
const float * const avxEndPtr(const float * const startPtr, const std::size_t elements) {
    assert(startPtr != nullptr);
    assert(elements > 0);

    return startPtr + avxRegistersNeeded(elements) * NUM_FLOATS_PER_AVX_REGISTER;
}

template<int i>
const __m256 broadcast(const __m256 value) {
    return _mm256_permute_ps(value, _MM_SHUFFLE(i, i, i, i));
}

const __m256 unpackLow(const __m256 packed) {
    return _mm256_permute2f128_ps(packed, packed, 0);
}

const __m256 unpackHigh(const __m256 packed) {
    return _mm256_permute2f128_ps(packed, packed, 1 + 1 << 4);
}

struct AVXVector
{
    std::size_t elements;

    // padded size of elements
    std::size_t padded;

    std::vector<float, AlignmentAllocator<float>> data;

    AVXVector(const std::size_t elements, const float initialValue = INITIAL_VALUE)
        :
        elements(elements),
        padded(padSize(elements)),
        data() {
        assert(elements > 0);

        this->data.resize(this->padded, PADDING_VALUE);

        for (auto i = 0; i < this->elements; ++i) {
            this->at(i) = initialValue;
        }
    }

    float & at(const std::size_t i) {
        return this->data.at(i);
    }

    const float & at(const std::size_t i) const {
        return this->data.at(i);
    }
};

// AOS representation of a scalar matrix
struct ScalarMatrix
{
    std::size_t rows;
    std::size_t columns;

    std::vector<float> data;

    ScalarMatrix(const std::size_t rows, const std::size_t columns)
        :
        rows(rows),
        columns(columns),
        data() {
        assert(rows > 0);
        assert(columns > 0);

        this->data.resize(this->rows * this->columns, INITIAL_VALUE);
    }

    float & at(const std::size_t r, const std::size_t c) {
        return this->data.at(r * this->columns + c);
    }

    const float & at(const std::size_t r, const std::size_t c) const {
        return this->data.at(r * this->columns + c);
    }
};

// multiplies a broadcast vector of one column element in the input vector element-wise with the
// column vector of a matrix (that is the row in the SOAMatrix).
// helps the SOAMatrix.avxTransform(const AVXVector &) to split some ocde.
struct TransformHelper
{
    std::size_t avxElementsPerColumn;
    const float * dataPtrStart;
    float * resultPtrStart;
    float * resultPtrEnd;

    TransformHelper(const std::size_t rows, const float * const dataPtrStart, float * const resultPtrStart)
        :
        avxElementsPerColumn(avxRegistersNeeded(rows) * NUM_FLOATS_PER_AVX_REGISTER),
        dataPtrStart(dataPtrStart),
        resultPtrStart(resultPtrStart),
        resultPtrEnd() {
        assert(rows > 0);
        assert(dataPtrStart != nullptr);
        assert(resultPtrStart != nullptr);

        this->resultPtrEnd = resultPtrStart + this->avxElementsPerColumn;
    }

    void operator()(const std::size_t column, __m256 inputBroadcast) const {
        assert(column >= 0);

        // Thanks to the SOA layout, each row in SOAMatrix contains AVX fitting partial vectors of
        // the elements of the corresponding column "c" in the AOS matrix, so that we can directly
        // load the data into fitting AVX registers, multiply the elements and add the result back
        // into our intermediate result vector.
        auto rowPtr = this->dataPtrStart + column * this->avxElementsPerColumn;
        for (auto resultPtr = this->resultPtrStart; resultPtr < this->resultPtrEnd; resultPtr += NUM_FLOATS_PER_AVX_REGISTER,
            rowPtr += NUM_FLOATS_PER_AVX_REGISTER) {
            auto partialColumn = _mm256_load_ps(rowPtr);
            auto partialResult = _mm256_load_ps(resultPtr);
            auto intermediate = _mm256_fmadd_ps(
                partialColumn, /* multiply by */ inputBroadcast,
                /* and add */ partialResult
            );
            _mm256_store_ps(resultPtr, intermediate);
        }
    }
};

struct SOAMatrix
{
    std::size_t rows;
    std::size_t columns;

    // for SOA structures, each column is padded by the "count of rows"
    std::size_t padded;

    std::vector<float, AlignmentAllocator<float>> data;

    SOAMatrix(const ScalarMatrix & original)
        :
        rows(original.rows),
        columns(original.columns),
        padded(padSize(original.rows)),
        data() {
        assert(original.rows > 0);
        assert(original.columns > 0);

        // for SOA structures, the rows (count of columns) are padded
        this->data.resize(this->padded * this->columns, PADDING_VALUE);

        // the following code looks like a simple copy operation of the data of the original AOSMatrix into this SOAMatrix
        // however, it is the conversion of the layout of the data from AOS to SOA by simply transposing it in memory.
        // thereby, the count of elements keeps the same as does the unpadded size of the data and the data itself.
        for (auto r = 0; r < this->rows; ++r) {
            for (auto c = 0; c < this->columns; ++c) {
                // see "float & at(std::size_t r, std::size_t c)" for details
                this->at(r, c) = original.at(r, c);
            }
        }
    }

    float & at(const std::size_t r, const std::size_t c) {
        return this->data.at(r + c * this->padded);
    }

    const float & at(const std::size_t r, const std::size_t c) const {
        return this->data.at(r + c * this->padded);
    }

    // implements the scalar version of the matrix-vector multiplication as template code
    const AVXVector avxTransform(const AVXVector & inputVector) const {
        assert(inputVector.elements == this->columns);

        // HERE is the MOST IMPORTANT code in this example.

        auto result = AVXVector(this->rows, 0.0f);
        auto transformHelper = TransformHelper(this->rows, this->data.data(), result.data.data());

        // remember that we have the SOA layout, so we iterate on the columns in the outer most loop.
        // in the inner loop, we vertically do the partial dot product step by step.
        auto column = 0;
        auto inputPtr = inputVector.data.data();
        auto inputPtrEnd = avxEndPtr(inputPtr, inputVector.elements);
        for (; inputPtr < inputPtrEnd; inputPtr += NUM_FLOATS_PER_AVX_REGISTER) {
            // thanks to the padding we can safely load partials of the data into one whole AVX register at once
            auto partialInput = _mm256_load_ps(inputPtr);

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

        auto result = AVXVector(this->rows, 0.0f);

        // remember that we have the SOA layout, so we iterate on the columns in the outer most loop.
        // in the inner loop we vertically do the partial dot product step by step.
        for (auto c = 0; c < this->columns; ++c) {
            for (auto r = 0; r < this->rows; ++r) {
                result.at(r) += this->at(r, c) * inputVector.at(c);
            }
        }

        return result;
    }

    // performs a RxC * Cx1->Rx1 matrix-vector multiplication
    const AVXVector transform(const AVXVector & inputVector) const {
        assert(inputVector.elements == this->columns);

        auto avxResult = this->avxTransform(inputVector);

        // verify correctness by comparing the scalar result against the result of the AVX computation
        auto scalarResult = this->scalarTransform(inputVector);
        assert(scalarResult.elements == avxResult.elements);
        for (auto i = 0; i < scalarResult.elements; ++i) {
            assert(scalarResult.at(i) == avxResult.at(i));
        }

        return avxResult;
    }
};

int main() {
    // Be cautious with the sizes here. They determine how much memory is used when running the code.
    // 10000x10000 already uses around 220 MB memory.
    // On my i7 7700k it runs within 25s in debug mode.
    // In release mode it is less than 1s.
    auto inputVectorSize = 5000;
    auto outputVectorSize = 5000;

    auto inputVector = AVXVector(inputVectorSize);
    auto aosTransform = ScalarMatrix(outputVectorSize, inputVectorSize);

    // 1: we use a "structure of arrays" memory layout for the transform matrix
    // so that we can compute the transform in SIMD preferred way
    auto soaTransform = SOAMatrix(aosTransform);

    // 2: transform the input vector to the output vector, see SOAMatrix.transform()
    auto outputVector = soaTransform.transform(inputVector);

    // 3: verify the result. in this example code, the result should be the count of elements of
    // the input vector set on the first two elements of the output vector as all vector and
    // matrix elements are set to 1.0f.
    for (auto i = 0; i < outputVectorSize; ++i) {
        std::cout << outputVector.at(i) << " ";
    }
}