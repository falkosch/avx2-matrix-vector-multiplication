#pragma once

#include "scalar-variant.h"

#include <array>
#include <vector>

namespace matrixmultiplication::avx2
{
    class alignas(32) AVXPack : public std::array<float, 8>
    {
    };

    class AVXVector
    {
        std::size_t _elements;
        std::vector<AVXPack> _packs;

      public:
        static constexpr float INITIAL_VALUE =
            matrixmultiplication::scalar::ScalarMatrix::INITIAL_VALUE;

        typedef std::vector<AVXPack>::const_iterator const_iterator;
        typedef std::vector<AVXPack>::iterator iterator;

        explicit AVXVector(const std::size_t elements,
                           const float initialValue = INITIAL_VALUE) noexcept;

        std::vector<AVXPack> & packs() noexcept;
        const std::vector<AVXPack> & packs() const noexcept;

        std::size_t size() const noexcept;

        float & at(const std::size_t i) noexcept;
        float at(const std::size_t i) const noexcept;
    };

    class SOAMatrix
    {
        std::size_t _rows;
        std::size_t _columns;

        // count of how many AVXPacks make up a full column
        std::size_t _rowsPaddedSize;

        std::vector<AVXPack> _packs;

      public:
        typedef std::vector<AVXPack>::const_iterator const_iterator;

        explicit SOAMatrix(const matrixmultiplication::scalar::ScalarMatrix &
                               original) noexcept;

        std::vector<AVXPack> & packs() noexcept;
        const std::vector<AVXPack> & packs() const noexcept;

        std::size_t rows() const noexcept;
        std::size_t columns() const noexcept;

        float & at(const std::size_t r, const std::size_t c) noexcept;
        float at(const std::size_t r, const std::size_t c) const noexcept;
    };

    // performs a RxC * Cx1 -> Rx1 matrix-vector multiplication
    AVXVector transform(const SOAMatrix & matrix,
                        const AVXVector & inputVector) noexcept;

}
