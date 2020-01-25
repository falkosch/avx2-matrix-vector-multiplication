#include "scalar-variant.h"

#include <cassert>

using namespace std;

namespace matrixmultiplication
{
    namespace scalar
    {
        ScalarMatrix::ScalarMatrix(const size_t rows, const size_t columns,
                                   const float initialValue)
            : rows{rows}, columns{columns},
              components(rows * columns, initialValue)
        {
            assert(rows > 0);
            assert(columns > 0);
        }

        float & ScalarMatrix::at(const size_t r, const size_t c)
        {
            return this->components.at(r * this->columns + c);
        }

        float ScalarMatrix::at(const size_t r, const size_t c) const
        {
            return this->components.at(r * this->columns + c);
        }
    }
}
