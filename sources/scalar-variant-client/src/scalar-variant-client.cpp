#include "scalar-variant-client.h"

#include <scalar-variant.h>

#include <cstdint>
#include <iostream>

using namespace std;
using namespace matrixmultiplication::scalar;

int main() noexcept
{
    // Be cautious with the sizes here. They determine how much memory is used
    // when running the code. 10000x10000 already uses around 220 MB memory.
    const size_t inputVectorSize{100};
    const size_t outputVectorSize{100};

    const ScalarMatrix soaMatrix{outputVectorSize, inputVectorSize, 1.0F};
    const ScalarVector inputVector(inputVectorSize, 1.0F);

    const auto outputVector = transform(soaMatrix, inputVector);

    for (size_t i{0}; i < outputVectorSize; ++i)
    {
        cout << outputVector.at(i) << " ";
    }
    cout << endl;

    return 0;
}
