#pragma once

#include <array>
#include <vector>

namespace matrixmultiplication::avx2
{
    // Value of elements added due to padding of the data sizes
    constexpr float PADDING_VALUE{-0.0F};

    // We have 8 single precision float elements in an AVX register
    constexpr size_t NUM_FLOATS_PER_AVX_REGISTER{8};

    // We pad lengths by the count of elements in the AVX registers
    // so that we have it easier to handle the vector data in the
    // transformation.
    size_t padSize(const size_t size) noexcept;

    // Represents a pack of content of what would fit into one AVX register and
    // enforces 32-byte alignment on it.
    class alignas(32) AVXPack : public std::array<float, 8>
    {
    };

    // Represents a dynamic size vector (as defined in mathematics) that has
    // padded elements to make it easier to load its data pack-wise into AVX
    // registers.
    class AVXVector
    {
        std::size_t _elements;
        std::vector<AVXPack> _packs;

      public:
        static constexpr float INITIAL_VALUE = 0.0F;

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

    // Represents a dynamic size matrix that has padded elements to make it
    // easier to load its data pack-wise into AVX registers.
    class SOAMatrix
    {
        std::size_t _rows;
        std::size_t _columns;

        // count of how many AVXPacks make up a full column
        std::size_t _rowsPaddedSize;

        std::vector<AVXPack> _packs;

      public:
        typedef std::vector<AVXPack>::const_iterator const_iterator;

        static constexpr float INITIAL_VALUE = 0.0F;

        explicit SOAMatrix(const std::size_t rows, const std::size_t columns,
                           const float initialValue = INITIAL_VALUE) noexcept;

        std::vector<AVXPack> & packs() noexcept;
        const std::vector<AVXPack> & packs() const noexcept;

        std::size_t rows() const noexcept;
        std::size_t columns() const noexcept;

        float & at(const std::size_t r, const std::size_t c) noexcept;
        float at(const std::size_t r, const std::size_t c) const noexcept;
    };
}