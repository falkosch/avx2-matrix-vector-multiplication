#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::avx2
{
    SCENARIO("modifiable AVX2 matrices")
    {
        GIVEN("an initial arbitrary matrix")
        {
            const SOAMatrix matrix{3, 2};

            WHEN("querying its value at (0, 0)")
            {
                const auto actualValue = matrix.at(0, 0);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }

            WHEN("querying its value at (2, 1)")
            {
                const auto actualValue = matrix.at(2, 1);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }
        }

        GIVEN("a matrix of same size but with different initial value")
        {
            const SOAMatrix matrix{3, 2, 1.0F};

            WHEN("querying its value at (0, 0)")
            {
                const auto actualValue = matrix.at(0, 0);

                THEN("it returns another initial value")
                {
                    REQUIRE(actualValue == 1.0F);
                }
            }

            WHEN("querying its value at (2, 1)")
            {
                const auto actualValue = matrix.at(2, 1);

                THEN("it returns another initial value")
                {
                    REQUIRE(actualValue == 1.0F);
                }
            }
        }

        GIVEN("a mutated matrix of same size")
        {
            const auto matrix = [] {
                SOAMatrix m{3, 2};
                m.at(0, 0) = 2.0F;
                m.at(1, 1) = 2.0F;
                return m;
            }();

            WHEN("querying modified value at (0, 0)")
            {
                const auto actualValue = matrix.at(0, 0);

                THEN("it returns the modified value")
                {
                    REQUIRE(actualValue == 2.0F);
                }
            }

            WHEN("querying not modified value at (2, 1)")
            {
                const auto actualValue = matrix.at(2, 1);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
