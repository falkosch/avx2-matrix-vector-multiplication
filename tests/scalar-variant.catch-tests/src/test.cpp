#include "test_commons.h"

namespace matrixmultiplication::scalar
{
    SCENARIO("scalar matrix multiplication")
    {
        GIVEN("a sample 2D scalar vector"
              "and a 2x2 scalar identity matrix")
        {
            const ScalarVector sampleVector{1.0F, 2.0F};

            const auto identityMatrix = []() {
                ScalarMatrix m{2, 2, 0.0F};
                m.at(0, 0) = 1.0F;
                m.at(1, 1) = 1.0F;
                return m;
            }();

            WHEN("vector is transformed using the matrix")
            {
                const ScalarVector result =
                    transform(identityMatrix, sampleVector);

                THEN("it results in a 2D scalar vector again")
                {
                    REQUIRE(result.size() == 2);
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
