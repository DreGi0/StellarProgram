/**
 * @file orbit_tests.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "physics/orbit.h"


TEST_CASE("gravity points to center on axis")
{
    const glm::dvec3 gAccel = Stellar::gravityAcceleration(glm::dvec3(10.0, 0.0, 0.0), 1000.0);

    REQUIRE(gAccel.x == Catch::Approx(-10.0));
    REQUIRE(gAccel.y == Catch::Approx(0.0));
    REQUIRE(gAccel.z == Catch::Approx(0.0));
}

TEST_CASE("in diagonal")
{
    const glm::dvec3 gAccel = Stellar::gravityAcceleration(glm::dvec3(3.0, 4.0, 0.0), 1000.0);

    REQUIRE(gAccel.x == Catch::Approx(-24.0));
    REQUIRE(gAccel.y == Catch::Approx(-32.0));
    REQUIRE(gAccel.z == Catch::Approx(0.0));
}
