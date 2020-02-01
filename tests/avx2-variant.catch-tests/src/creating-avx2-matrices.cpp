#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::avx2
{
    SCENARIO("creating AVX2 matrices")
    {
        GIVEN("an empty matrix")
        {
            const SOAMatrix matrix{0, 0};

            WHEN("querying the rows count")
            {
                const auto actualRows = matrix.rows();

                THEN("it returns 0")
                {
                    REQUIRE(actualRows == 0);
                }
            }

            WHEN("querying the columns count")
            {
                const auto actualColumns = matrix.columns();

                THEN("it returns 0")
                {
                    REQUIRE(actualColumns == 0);
                }
            }
        }

        GIVEN("an 1x1 matrix")
        {
            const SOAMatrix matrix{1, 1};

            WHEN("querying the rows count")
            {
                const auto actualRows = matrix.rows();

                THEN("it returns 1")
                {
                    REQUIRE(actualRows == 1);
                }
            }

            WHEN("querying the columns count")
            {
                const auto actualColumns = matrix.columns();

                THEN("it returns 1")
                {
                    REQUIRE(actualColumns == 1);
                }
            }
        }

        GIVEN("any squared matrix")
        {
            const SOAMatrix matrix{100, 100};

            WHEN("querying the rows count")
            {
                const auto actualRows = matrix.rows();

                THEN("it returns the rows count it was created with")
                {
                    REQUIRE(actualRows == 100);
                }
            }

            WHEN("querying the columns count")
            {
                const auto actualColumns = matrix.columns();

                THEN("it returns the columns count it was created with")
                {
                    REQUIRE(actualColumns == 100);
                }
            }
        }

        GIVEN("any arbitrary matrix")
        {
            const SOAMatrix matrix{75, 3};

            WHEN("querying the rows count")
            {
                const auto actualRows = matrix.rows();

                THEN("it returns the rows count it was created with")
                {
                    REQUIRE(actualRows == 75);
                }
            }

            WHEN("querying the columns count")
            {
                const auto actualColumns = matrix.columns();

                THEN("it returns the columns count it was created with")
                {
                    REQUIRE(actualColumns == 3);
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
