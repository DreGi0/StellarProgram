/**
 * @file world.cpp
 * @brief World simulation rules: stepping, time warp and burns.
 * @author DreGi0
 * @date October 2nd, 2026
 */

#include "world.h"

#include <glm/trigonometric.hpp>
#include <glm/common.hpp>

constexpr double THRUST_ACCELERATION = 1.0;
constexpr double MAX_PHYSICS_WARP = 4.0;

// Starting orbit, described with elements instead of position + velocity
constexpr Stellar::OrbitalElements START_ORBIT {
    .semiMajorAxis = 10.0,
    .eccentricity = 0.3,
    .inclination = glm::radians(20.0),
    .longitudeOfAscendingNode = 0.0,
    .argumentOfPeriapsis = 0.0,
    .trueAnomaly = 0.0,
};

// note: it only needs one (Velocity Verlet), but since I'm trying to see what happens on each
constexpr Stellar::StepFunction INTEGRATORS[] = {
    Stellar::stepExplicitEuler,
    Stellar::stepSemiImplicitEuler,
    Stellar::stepVelocityVerlet,
    Stellar::stepKepler,
};

constexpr double TIME_WARP_LEVELS[] = {
    1.0, 5.0, 10.0, 50.0, 100.0, 1'000.0, 10'000.0, 100'000.0
};

namespace Stellar
{
    World::World()
    {
        reset();
    }

    void World::update(const double deltaTime)
    {
        m_vessel.previousPosition = m_vessel.state.position;

        if (isBurning())
        {
            // Engine on: the orbit is changing, so leave the rails and step the physics with Verlet
            const glm::dvec3 thrust = THRUST_ACCELERATION * burnDirection(m_vessel.state, m_vessel.burn.x, m_vessel.burn.y);

            m_vessel.state = stepVelocityVerletWithThrust(m_vessel.state, ORBIT_MU, deltaTime, thrust);
            m_missionTime += deltaTime;

            // The burn changes the energy on purpose; measure drift from the new orbit instead
            m_initialEnergy = specificEnergy(m_vessel.state, ORBIT_MU);
            return;
        }

        const double warpedDt = deltaTime * timeWarp();

        m_vessel.state = INTEGRATORS[m_integratorIndex](m_vessel.state, ORBIT_MU, warpedDt);
        m_missionTime += warpedDt;
    }

    void World::reset()
    {
        m_vessel.state = elementsToState(START_ORBIT, ORBIT_MU);
        m_vessel.previousPosition = m_vessel.state.position;
        m_initialEnergy = specificEnergy(m_vessel.state, ORBIT_MU);

        m_missionTime = 0.0;
    }

    Vessel& World::vessel()
    {
        return m_vessel;
    }

    const Vessel& World::vessel() const
    {
        return m_vessel;
    }

    void World::setIntegrator(const int index)
    {
        m_integratorIndex = index;

        reset();
    }

    int World::integratorIndex() const
    {
        return m_integratorIndex;
    }

    void World::setTimeWarp(const int index)
    {
        m_timeWarpIndex = index;
    }

    int World::timeWarpIndex() const
    {
        return m_timeWarpIndex;
    }

    // Big steps would wreck the step-by-step integrators; only Kepler is exact for any step
    double World::timeWarp() const
    {
        return isOnRails() && !isBurning() ? TIME_WARP_LEVELS[m_timeWarpIndex] : 1.0;
    }

    double World::physicsWarp() const
    {
        return isBurning() ? glm::min(TIME_WARP_LEVELS[m_timeWarpIndex], MAX_PHYSICS_WARP) : 1.0;
    }

    double World::missionTime() const
    {
        return m_missionTime;
    }

    double World::initialEnergy() const
    {
        return m_initialEnergy;
    }

    bool World::isOnRails() const
    {
        return INTEGRATORS[m_integratorIndex] == stepKepler;
    }

    bool World::isBurning() const
    {
        return m_vessel.burn != glm::dvec2(0.0);
    }
} // Stellar