#pragma once

#include <vector>

namespace matrixmultiplication::scalar
{
    typedef std::vector<float> ScalarVector;

    // AOS representation of a matrix
    class ScalarMatrix
    {
        std::size_t _rows;
        std::size_t _columns;

        std::vector<float> _components;

      public:
        static constexpr float INITIAL_VALUE = 0.0F;

        explicit ScalarMatrix(
            const std::size_t rows, const std::size_t columns,
            const float initialValue = INITIAL_VALUE) noexcept;

        std::size_t rows() const noexcept;
        std::size_t columns() const noexcept;

        float & at(const std::size_t r, const std::size_t c) noexcept;
        float at(const std::size_t r, const std::size_t c) const noexcept;
    };

    ScalarVector transform(const ScalarMatrix & matrix,
                           const ScalarVector & inputVector) noexcept;
}
