#pragma once

#include "scalar-variant.h"

#include <array>
#include <vector>

#if defined(__GNUC__)
#include <x86intrin.h>
#else
#include <intrin.h>
#endif

namespace matrixmultiplication
{
    namespace avx2
    {
        union alignas(32) AVXPack {
            __m256 packed;
            std::array<float, 8> components;

            explicit AVXPack(const __m256 & packed);
        };

        struct AVXVector
        {
            std::size_t elements;
            std::vector<AVXPack> packs;

            explicit AVXVector(
                const std::size_t elements,
                const float initialValue =
                    matrixmultiplication::scalar::ScalarMatrix::INITIAL_VALUE);

            float & at(const std::size_t i);

            float at(const std::size_t i) const;
        };

        struct SOAMatrix
        {
            std::size_t rows;
            std::size_t columns;

            // count of how many AVXPacks make up a full column
            std::size_t rowsPaddedSize;

            std::vector<AVXPack> packs;

            explicit SOAMatrix(
                const matrixmultiplication::scalar::ScalarMatrix & original);

            float & at(const std::size_t r, const std::size_t c);

            float at(const std::size_t r, const std::size_t c) const;

            // implements the scalar version of the matrix-vector multiplication
            // as template code
            const AVXVector avxTransform(const AVXVector & inputVector) const;

            // implements the scalar version of the matrix-vector multiplication
            // as template code
            const AVXVector scalarTransform(
                const AVXVector & inputVector) const;

            // performs a RxC * Cx1->Rx1 matrix-vector multiplication
            const AVXVector transform(const AVXVector & inputVector) const;
        };
    }
}
