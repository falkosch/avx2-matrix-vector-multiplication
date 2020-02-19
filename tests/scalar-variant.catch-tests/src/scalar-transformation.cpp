#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::scalar
{
    SCENARIO("scalar transformation")
    {
        GIVEN("a 1D vector and a 1x1 matrix")
        {
            const ScalarVector sampleVector{1.0F};
            const ScalarMatrix identityMatrix{1, 1, 1.0F};

            WHEN("transforming the vector by the matrix")
            {
                const auto result = transform(identityMatrix, sampleVector);

                THEN("it results in a 1D vector again")
                {
                    REQUIRE(result.size() == 1);
                }

                THEN("the resulting vector is equal to the given vector")
                {
                    REQUIRE_THAT(result, Equals(sampleVector));
                }
            }
        }

        GIVEN("a 4D vector and a 4x4 identity matrix")
        {
            const ScalarVector sampleVector{1.0F, 2.0F, 3.0F, 4.0F};

            const auto identityMatrix = []() {
                ScalarMatrix m{4, 4};
                m.at(0, 0) = 1.0F;
                m.at(1, 1) = 1.0F;
                m.at(2, 2) = 1.0F;
                m.at(3, 3) = 1.0F;
                return m;
            }();

            WHEN("transforming the vector by the matrix")
            {
                const auto result = transform(identityMatrix, sampleVector);

                THEN("it results in a 4D vector again")
                {
                    REQUIRE(result.size() == 4);
                }

                THEN("the resulting vector is equal to the given vector")
                {
                    REQUIRE_THAT(result, Equals(sampleVector));
                }
            }
        }

        GIVEN("a 10D vector and a 9x10 matrix")
        {
            const ScalarVector sampleVector{1.0F, 2.0F, 3.0F, 4.0F, 5.0F,
                                            6.0F, 7.0F, 8.0F, 9.0F, 10.0F};

            const auto identityMatrix = []() {
                ScalarMatrix m{9, 10};
                m.at(0, 0) = 1.0F;
                m.at(1, 1) = 1.0F;
                m.at(2, 2) = 1.0F;
                m.at(3, 3) = 1.0F;
                m.at(4, 4) = 1.0F;
                m.at(5, 5) = 1.0F;
                m.at(6, 6) = 1.0F;
                m.at(7, 7) = 1.0F;
                m.at(8, 8) = 1.0F;
                return m;
            }();

            WHEN("transforming the vector by the matrix")
            {
                const auto result = transform(identityMatrix, sampleVector);

                THEN("it results in a 9D vector again")
                {
                    REQUIRE(result.size() == 9);
                }

                THEN("the resulting vector is equal to the first 9 components "
                     "of the given vector")
                {
                    const ScalarVector expectedVector{
                        1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F};

                    REQUIRE_THAT(result, Equals(expectedVector));
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
