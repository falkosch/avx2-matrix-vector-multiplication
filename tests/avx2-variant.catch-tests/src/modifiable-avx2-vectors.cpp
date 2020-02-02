#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::avx2
{
    SCENARIO("modifiable AVX2 vectors")
    {
        GIVEN("an initial arbitrary vector")
        {
            const AVXVector vector(3);

            WHEN("querying its value at 0")
            {
                const auto actualValue = vector.at(0);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }

            WHEN("querying its value at 2")
            {
                const auto actualValue = vector.at(2);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }
        }

        GIVEN("a vector of same size but with different initial value")
        {
            const AVXVector vector(3, 1.0F);

            WHEN("querying its value at 0")
            {
                const auto actualValue = vector.at(0);

                THEN("it returns another initial value")
                {
                    REQUIRE(actualValue == 1.0F);
                }
            }

            WHEN("querying its value at 2")
            {
                const auto actualValue = vector.at(2);

                THEN("it returns another initial value")
                {
                    REQUIRE(actualValue == 1.0F);
                }
            }
        }

        GIVEN("a mutated vector of same size")
        {
            const auto vector = [] {
                AVXVector v(3);
                v.at(0) = 2.0F;
                v.at(1) = 2.0F;
                return v;
            }();

            WHEN("querying modified value at 0")
            {
                const auto actualValue = vector.at(0);

                THEN("it returns the modified value")
                {
                    REQUIRE(actualValue == 2.0F);
                }
            }

            WHEN("querying not modified value at 2")
            {
                const auto actualValue = vector.at(2);

                THEN("it returns the default initial value")
                {
                    REQUIRE(actualValue == 0.0F);
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
