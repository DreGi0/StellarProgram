/**
 * @file orbit_tests.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include <cmath>
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "physics/orbit.h"

namespace
{
    using StepFunction = Stellar::OrbitalState (*)(const Stellar::OrbitalState&, double, double);

    double maxEnergyDrift(const StepFunction step)
    {
        constexpr double mu = 1000.0;
        constexpr double dt = 1.0 / 60.0;

        Stellar::OrbitalState state{glm::dvec3(10.0, 0.0, 0.0), glm::dvec3(0.0, 10.0, 0.0)};

        const double initialEnergy = Stellar::specificEnergy(state, mu);
        constexpr auto period = glm::two_pi<double>();
        const int steps = static_cast<int>(std::round(10.0 * period / dt));

        double maxDrift = 0.0;

        for (int i = 0; i < steps; ++i)
        {
            state = step(state, mu, dt);

            const double energy = Stellar::specificEnergy(state, mu);
            const double drift = std::abs(energy - initialEnergy) / std::abs(initialEnergy);

            maxDrift = std::max(maxDrift, drift);
        }

        return maxDrift;
    }
}

TEST_CASE("gravity points to center on axis")
{
    const glm::dvec3 gAccel = Stellar::gravityAcceleration(glm::dvec3(10.0, 0.0, 0.0), 1000.0);

    REQUIRE(gAccel.x == Catch::Approx(-10.0));
    REQUIRE(gAccel.y == Catch::Approx(0.0));
    REQUIRE(gAccel.z == Catch::Approx(0.0));
}

TEST_CASE("gravity points to center on diagonal")
{
    const glm::dvec3 gAccel = Stellar::gravityAcceleration(glm::dvec3(3.0, 4.0, 0.0), 1000.0);

    REQUIRE(gAccel.x == Catch::Approx(-24.0));
    REQUIRE(gAccel.y == Catch::Approx(-32.0));
    REQUIRE(gAccel.z == Catch::Approx(0.0));
}

TEST_CASE("explicit Euler gains energy on a circular orbit")
{
    REQUIRE(maxEnergyDrift(Stellar::stepExplicitEuler) > 0.1);
}

TEST_CASE("semi-implicit Euler keeps energy bounded")
{
    REQUIRE(maxEnergyDrift(Stellar::stepSemiImplicitEuler) < 1e-3);
}

TEST_CASE("velocity Verlet keeps energy almost constant")
{
    REQUIRE(maxEnergyDrift(Stellar::stepVelocityVerlet) < 1e-6);
}
