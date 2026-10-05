/**
 * @file world_tests.cpp
 * @brief Rules of World: initial state, warp limits while burning and reset on integrator change.
 * @author DreGi0
 * @date October 2nd, 2026
 */

#include <catch2/catch_test_macros.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <glm/gtc/type_precision.hpp>

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

TEST_CASE("A new World holds the vessel as its first object")
{
    const Stellar::World world;

    REQUIRE(world.objects().size() == 1);
    CHECK(&world.vessel() == &world.objects().front());
    CHECK(world.vessel().vessel.has_value());
}

TEST_CASE("Falling onto the planet leaves the vessel resting on the surface")
{
    Stellar::World world;
    constexpr double dt = 1.0 / 60.0;
    const double contactRadius = Stellar::PLANET_RADIUS + world.vessel().transform.scale;

    // Burn retrograde until touchdown (step limit so a bug can't hang the test)
    world.setBurn(glm::dvec2(-1.0, 0.0));

    for (int i = 0; i < 100'000 && glm::length(world.vessel().transform.position) > contactRadius + 1e-9; ++i)
    {
        world.update(dt);
    }

    world.setBurn(glm::dvec2(0.0)); // Engine off so the vessel can rest

    const glm::dvec3 landedPosition = world.vessel().transform.position;
    REQUIRE(glm::length(landedPosition) <= contactRadius + 1e-9);

    // Stays put, no NaN, and time warp is blocked (periapsis is under the surface)
    world.setTimeWarp(7);

    for (int i = 0; i < 100; ++i)
    {
        world.update(dt);
    }

    const glm::dvec3 position = world.vessel().transform.position;
    CHECK_FALSE(glm::any(glm::isnan(position)));
    CHECK(position == landedPosition);
    CHECK(world.vessel().velocity == glm::dvec3(0.0));
    CHECK(world.timeWarp() == 1.0);
}

TEST_CASE("Burning from the ground keeps the vessel finite")
{
    Stellar::World world;
    constexpr double dt = 1.0 / 60.0;
    const double contactRadius = Stellar::PLANET_RADIUS + world.vessel().transform.scale;

    world.setBurn(glm::dvec2(-1.0, 0.0));

    for (int i = 0; i < 100'000 && glm::length(world.vessel().transform.position) > contactRadius + 1e-9; ++i)
    {
        world.update(dt);
    }

    REQUIRE(glm::length(world.vessel().transform.position) <= contactRadius + 1e-9);

    // Every burn direction while landed (v = 0): prograde, retrograde, normal
    for (const glm::dvec2 burn : {glm::dvec2(1.0, 0.0), glm::dvec2(-1.0, 0.0), glm::dvec2(0.0, 1.0)})
    {
        world.setBurn(burn);
        world.update(dt);

        CHECK_FALSE(glm::any(glm::isnan(world.vessel().transform.position)));
        CHECK_FALSE(glm::any(glm::isnan(world.vessel().velocity)));
        CHECK(glm::length(world.vessel().transform.position) >= contactRadius - 1e-9); // thrust < gravity: stays on the ground
    }
}
