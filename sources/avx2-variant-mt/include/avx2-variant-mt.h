#pragma once

#include <avx2-model.h>

namespace matrixmultiplication::avx2
{
    // Performs a RxC * Cx1 -> Rx1 matrix-vector multiplication multi-threaded.
    // Uses OpenMP to parallelize the outer-loop iterations.
    AVXVector transformMultiThreaded(const SOAMatrix & matrix,
                                     const AVXVector & inputVector) noexcept;
}
