#include "test_commons.h"

using Catch::Matchers::Equals;

namespace matrixmultiplication::avx2
{
    SCENARIO("AVX2 transformation multi-threaded")
    {
        GIVEN("a 1D vector and a 1x1 matrix")
        {
            const AVXVector sampleVector(1, 1.0F);
            const SOAMatrix identityMatrix{1, 1, 1.0F};

            WHEN("transforming the vector by the matrix")
            {
                const auto result =
                    transformMultiThreaded(identityMatrix, sampleVector);

                THEN("it results in a 1D vector again")
                {
                    REQUIRE(result.size() == 1);
                }

                THEN("the resulting vector is equal to the given vector")
                {
                    REQUIRE_THAT(result.packs(), Equals(sampleVector.packs()));
                }
            }
        }

        GIVEN("a pack-size fitting vector and a pack-size fitting identity "
              "matrix")
        {
            const auto sampleVector = []() {
                AVXVector v(8);
                v.at(0) = 1.0F;
                v.at(1) = 2.0F;
                v.at(2) = 3.0F;
                v.at(3) = 4.0F;
                v.at(4) = 5.0F;
                v.at(5) = 6.0F;
                v.at(6) = 7.0F;
                v.at(7) = 8.0F;
                return v;
            }();

            const auto identityMatrix = []() {
                SOAMatrix m{8, 8};
                m.at(0, 0) = 1.0F;
                m.at(1, 1) = 1.0F;
                m.at(2, 2) = 1.0F;
                m.at(3, 3) = 1.0F;
                m.at(4, 4) = 1.0F;
                m.at(5, 5) = 1.0F;
                m.at(6, 6) = 1.0F;
                m.at(7, 7) = 1.0F;
                return m;
            }();

            WHEN("transforming the vector is transformed by the matrix")
            {
                const auto result =
                    transformMultiThreaded(identityMatrix, sampleVector);

                THEN("it results in a 8D vector again")
                {
                    REQUIRE(result.size() == 8);
                }

                THEN("the resulting vector is equal to the given vector")
                {
                    REQUIRE_THAT(result.packs(), Equals(sampleVector.packs()));
                }
            }
        }

        GIVEN("a 10D vector and a 9x10 matrix")
        {
            const auto sampleVector = []() {
                AVXVector v(10);
                v.at(0) = 1.0F;
                v.at(1) = 2.0F;
                v.at(2) = 3.0F;
                v.at(3) = 4.0F;
                v.at(4) = 5.0F;
                v.at(5) = 6.0F;
                v.at(6) = 7.0F;
                v.at(7) = 8.0F;
                v.at(8) = 9.0F;
                v.at(9) = 10.0F;
                return v;
            }();

            const auto identityMatrix = []() {
                SOAMatrix m{9, 10};
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
                const auto result =
                    transformMultiThreaded(identityMatrix, sampleVector);

                THEN("it results in a 9D vector again")
                {
                    REQUIRE(result.size() == 9);
                }

                THEN("the resulting vector is equal to the first 9 components "
                     "of the given vector")
                {
                    const auto expectedVector = []() {
                        AVXVector v(9);
                        v.at(0) = 1.0F;
                        v.at(1) = 2.0F;
                        v.at(2) = 3.0F;
                        v.at(3) = 4.0F;
                        v.at(4) = 5.0F;
                        v.at(5) = 6.0F;
                        v.at(6) = 7.0F;
                        v.at(7) = 8.0F;
                        v.at(8) = 9.0F;
                        return v;
                    }();

                    REQUIRE_THAT(result.packs(),
                                 Equals(expectedVector.packs()));
                }
            }
        }
    }
} // namespace matrixmultiplication::scalar
