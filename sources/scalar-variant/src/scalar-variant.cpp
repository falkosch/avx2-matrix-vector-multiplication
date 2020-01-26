#include "scalar-variant.h"

#include <cassert>

using namespace std;

namespace matrixmultiplication::scalar
{
    ScalarMatrix::ScalarMatrix(const size_t rows, const size_t columns,
                               const float initialValue) noexcept
        : _rows{rows}, _columns{columns},
          _components(rows * columns, initialValue)
    {
        assert(rows > 0);
        assert(columns > 0);
    }

    std::size_t ScalarMatrix::rows() const noexcept
    {
        return this->_rows;
    }

    std::size_t ScalarMatrix::columns() const noexcept
    {
        return this->_columns;
    }

    float & ScalarMatrix::at(const size_t r, const size_t c) noexcept
    {
        return this->_components.at(r * this->_columns + c);
    }

    float ScalarMatrix::at(const size_t r, const size_t c) const noexcept
    {
        return this->_components.at(r * this->_columns + c);
    }

    std::vector<float> transform(
        const ScalarMatrix & matrix,
        const std::vector<float> & inputVector) noexcept
    {
        assert(inputVector.size() == matrix.columns());

        const auto rows = matrix.rows();
        const auto columns = matrix.columns();

        std::vector<float> result(rows);

        for (size_t r{0}; r < rows; ++r)
        {
            auto sum = 0.0F;

            for (size_t c{0}; c < columns; ++c)
            {
                sum += matrix.at(r, c) * inputVector.at(c);
            }

            result.at(r) = sum;
        }

        return result;
    }
}
