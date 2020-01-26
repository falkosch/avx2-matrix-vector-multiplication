#pragma once

#include <vector>

namespace matrixmultiplication::scalar
{
    // AOS representation of a matrix
    class ScalarMatrix
    {
        std::size_t _rows;
        std::size_t _columns;

        std::vector<float> _components;

      public:
        static constexpr float INITIAL_VALUE = 1.0F;

        explicit ScalarMatrix(
            const std::size_t rows, const std::size_t columns,
            const float initialValue = INITIAL_VALUE) noexcept;

        std::size_t rows() const noexcept;
        std::size_t columns() const noexcept;

        float & at(const std::size_t r, const std::size_t c) noexcept;
        float at(const std::size_t r, const std::size_t c) const noexcept;
    };

    std::vector<float> transform(
        const ScalarMatrix & matrix,
        const std::vector<float> & inputVector) noexcept;
}
