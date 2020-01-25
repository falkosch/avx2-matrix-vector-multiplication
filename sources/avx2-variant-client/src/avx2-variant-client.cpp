#include "avx2-variant-client.h"

#include <avx2-variant.h>
#include <scalar-variant.h>

#include <cstdint>
#include <iostream>

using namespace std;
using namespace matrixmultiplication::scalar;
using namespace matrixmultiplication::avx2;

int main()
{
    // Be cautious with the sizes here. They determine how much memory is used
    // when running the code. 10000x10000 already uses around 220 MB memory. On
    // my i7 7700k it runs within 25s in debug mode. In release mode it is less
    // than 1s.
    const size_t inputVectorSize{10000};
    const size_t outputVectorSize{10000};

    const ScalarMatrix aosTransform{outputVectorSize, inputVectorSize};

    const AVXVector inputVector{inputVectorSize};

    // 1: we use a "structure of arrays" memory layout for the transform matrix
    // so that we can compute the transform in SIMD preferred way
    const SOAMatrix soaTransform{aosTransform};

    // 2: transform the input vector to the output vector, see
    // SOAMatrix.transform()
    const auto outputVector = soaTransform.transform(inputVector);

    // 3: verify the result. in this example code, the result should be the
    // count of elements of the input vector set on the first two elements of
    // the output vector as all vector and matrix elements are set to 1.0f.
    for (size_t i{0}; i < outputVectorSize; ++i)
    {
        cout << outputVector.at(i) << " ";
    }

    return 0;
}
