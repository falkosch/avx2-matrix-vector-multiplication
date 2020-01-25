#pragma once

#include <vector>

namespace matrixmultiplication
{
    namespace scalar
    {
        // AOS representation of a matrix
        struct ScalarMatrix
        {
            static constexpr float INITIAL_VALUE = 1.0f;

            std::size_t rows;
            std::size_t columns;

            std::vector<float> components;

            explicit ScalarMatrix(const std::size_t rows,
                                  const std::size_t columns,
                                  const float initialValue = INITIAL_VALUE);

            float & at(const std::size_t r, const std::size_t c);

            float at(const std::size_t r, const std::size_t c) const;
        };
    }
}
