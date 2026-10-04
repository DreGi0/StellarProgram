/**
 * @file world_tests.cpp
 * @brief Rules of World: initial state, warp limits while burning and reset on integrator change.
 * @author DreGi0
 * @date October 2nd, 2026
 */

#include <catch2/catch_test_macros.hpp>
#include <glm/vec2.hpp>

#include "game/world.h"

TEST_CASE("A new World starts at mission time zero, on rails")
{
    const Stellar::World world;

    CHECK(world.missionTime() == 0.0);
    CHECK(world.isOnRails());
    CHECK_FALSE(world.isBurning());
}

TEST_CASE("Burning disables time warp and caps physics warp")
{
    Stellar::World world;

    world.setTimeWarp(7); // x100000, the highest level
    world.setBurn(glm::dvec2(1.0, 0.0));

    CHECK(world.isBurning());
    CHECK(world.timeWarp() == 1.0);
    CHECK(world.physicsWarp() == 4.0);
}

TEST_CASE("Changing the integrator resets the mission")
{
    Stellar::World world;

    for (int i = 0; i < 10; ++i)
    {
        world.update(1.0 / 60.0);
    }

    REQUIRE(world.missionTime() > 0.0);

    world.setIntegrator(0); // Explicit Euler

    CHECK(world.missionTime() == 0.0);
    CHECK_FALSE(world.isOnRails());
}