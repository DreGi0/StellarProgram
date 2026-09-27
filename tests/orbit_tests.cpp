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
#include <glm/trigonometric.hpp>

#include "physics/orbit.h"

namespace
{
    double maxEnergyDrift(const Stellar::StepFunction step)
    {
        constexpr double mu = 1000.0;
        constexpr double dt = 1.0 / 60.0;

        Stellar::OrbitalState state{
            .position = glm::dvec3(10.0, 0.0, 0.0),
            .velocity = glm::dvec3(0.0, 10.0, 0.0)
        };

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

TEST_CASE("circular equatorial orbit has zero eccentricity and inclination")
{
    constexpr Stellar::OrbitalState state{
        .position = glm::dvec3(10.0, 0.0, 0.0),
        .velocity = glm::dvec3(0.0, 10.0, 0.0)
    };

    const Stellar::OrbitalElements elements = Stellar::stateToElements(state, 1000.0);

    REQUIRE(elements.semiMajorAxis == Catch::Approx(10.0));
    REQUIRE(elements.eccentricity == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(elements.inclination == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(elements.longitudeOfAscendingNode == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(elements.argumentOfPeriapsis == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(elements.trueAnomaly == Catch::Approx(0.0).margin(1e-12));
}

TEST_CASE("state to elements matches Curtis example 4.3")
{
    // Earth orbit from the textbook, units: km and km/s
    constexpr Stellar::OrbitalState state{
        .position = glm::dvec3(-6045.0, -3490.0, 2500.0),
        .velocity = glm::dvec3(-3.457, 6.618, 2.533)
    };

    const Stellar::OrbitalElements elements = Stellar::stateToElements(state, 398600.0);

    REQUIRE(elements.semiMajorAxis == Catch::Approx(8788.0).epsilon(1e-3));
    REQUIRE(elements.eccentricity == Catch::Approx(0.1712).epsilon(1e-3));
    REQUIRE(glm::degrees(elements.inclination) == Catch::Approx(153.2).epsilon(1e-3));
    REQUIRE(glm::degrees(elements.longitudeOfAscendingNode) == Catch::Approx(255.3).epsilon(1e-3));
    REQUIRE(glm::degrees(elements.argumentOfPeriapsis) == Catch::Approx(20.07).epsilon(1e-3));
    REQUIRE(glm::degrees(elements.trueAnomaly) == Catch::Approx(28.45).epsilon(1e-3));
}

TEST_CASE("circular equatorial elements give the scene's starting state")
{
    constexpr Stellar::OrbitalElements elements{
        .semiMajorAxis = 10.0,
        .eccentricity = 0.0,
        .inclination = 0.0,
        .longitudeOfAscendingNode = 0.0,
        .argumentOfPeriapsis = 0.0,
        .trueAnomaly = 0.0
    };

    const Stellar::OrbitalState state = Stellar::elementsToState(elements, 1000.0);

    REQUIRE(state.position.x == Catch::Approx(10.0));
    REQUIRE(state.position.y == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(state.position.z == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(state.velocity.x == Catch::Approx(0.0).margin(1e-12));
    REQUIRE(state.velocity.y == Catch::Approx(10.0));
    REQUIRE(state.velocity.z == Catch::Approx(0.0).margin(1e-12));
}

TEST_CASE("elements -> state -> elements round trip")
{
    constexpr Stellar::OrbitalElements original{
        .semiMajorAxis = 20.0,
        .eccentricity = 0.3,
        .inclination = 0.5,
        .longitudeOfAscendingNode = 1.0,
        .argumentOfPeriapsis = 2.0,
        .trueAnomaly = 0.7
    };

    const Stellar::OrbitalElements result = Stellar::stateToElements(Stellar::elementsToState(original, 1000.0), 1000.0);

    REQUIRE(result.semiMajorAxis == Catch::Approx(original.semiMajorAxis));
    REQUIRE(result.eccentricity == Catch::Approx(original.eccentricity));
    REQUIRE(result.inclination == Catch::Approx(original.inclination));
    REQUIRE(result.longitudeOfAscendingNode == Catch::Approx(original.longitudeOfAscendingNode));
    REQUIRE(result.argumentOfPeriapsis == Catch::Approx(original.argumentOfPeriapsis));
    REQUIRE(result.trueAnomaly == Catch::Approx(original.trueAnomaly));
}

TEST_CASE("state -> elements -> state round trip (Curtis example 4.3)")
{
    constexpr Stellar::OrbitalState original{
        .position = glm::dvec3(-6045.0, -3490.0, 2500.0),
        .velocity = glm::dvec3(-3.457, 6.618, 2.533)
    };

    const Stellar::OrbitalState result = Stellar::elementsToState(Stellar::stateToElements(original, 398600.0), 398600.0);

    REQUIRE(result.position.x == Catch::Approx(original.position.x));
    REQUIRE(result.position.y == Catch::Approx(original.position.y));
    REQUIRE(result.position.z == Catch::Approx(original.position.z));
    REQUIRE(result.velocity.x == Catch::Approx(original.velocity.x));
    REQUIRE(result.velocity.y == Catch::Approx(original.velocity.y));
    REQUIRE(result.velocity.z == Catch::Approx(original.velocity.z));
}
