#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::avx2
{
    SCENARIO("scalable AVX2 vectors")
    {
        GIVEN("a 1D vector")
        {
            const AVXVector vector(1);

            WHEN("querying the size")
            {
                const auto actualSize = vector.size();

                THEN("it returns 1")
                {
                    REQUIRE(actualSize == 1);
                }
            }

            WHEN("querying the individual pack size")
            {
                const auto actualSize = vector.packs().size();

                THEN("it returns 1")
                {
                    REQUIRE(actualSize == 1);
                }
            }
        }

        GIVEN("pack-size fitting vector")
        {
            const AVXVector vector(8);

            WHEN("querying the size")
            {
                const auto actualSize = vector.size();

                THEN("it returns 8")
                {
                    REQUIRE(actualSize == 8);
                }
            }

            WHEN("querying the individual pack size")
            {
                const auto actualSize = vector.packs().size();

                THEN("it returns 1")
                {
                    REQUIRE(actualSize == 1);
                }
            }
        }

        GIVEN("any arbitrary vector")
        {
            const AVXVector vector(75);

            WHEN("querying the size")
            {
                const auto actualSize = vector.size();

                THEN("it returns 75")
                {
                    REQUIRE(actualSize == 75);
                }
            }

            WHEN("querying the individual pack size")
            {
                const auto actualSize = vector.packs().size();

                THEN("it returns 10")
                {
                    REQUIRE(actualSize == 10);
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
