#pragma once

#include <avx2-model.h>

namespace matrixmultiplication::avx2
{
    // Performs a RxC * Cx1 -> Rx1 matrix-vector multiplication.
    AVXVector transform(const SOAMatrix & matrix,
                        const AVXVector & inputVector) noexcept;
}