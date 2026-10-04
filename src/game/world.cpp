/**
 * @file world.cpp
 * @brief World simulation rules: stepping every object, time warp and burns.
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
        const double dt = deltaTime * timeWarp();

        for (GameObject& object : m_objects)
        {
            object.previousPosition = object.transform.position;
            OrbitalState state = orbitalState(object);

            if (object.vessel && object.vessel->burn != glm::dvec2(0.0))
            {
                // Engine on: the orbit is changing, so leave the rails and step with Verlet
                const glm::dvec3 thrust = THRUST_ACCELERATION * burnDirection(state, object.vessel->burn.x, object.vessel->burn.y);
                state = stepVelocityVerletWithThrust(state, ORBIT_MU, dt, thrust);
            }
            else
            {
                state = INTEGRATORS[m_integratorIndex](state, ORBIT_MU, dt);
            }

            object.transform.position = state.position;
            object.velocity = state.velocity;
        }

        m_missionTime += dt;

        if (isBurning())
        {
            m_initialEnergy = specificEnergy(orbitalState(vessel()), ORBIT_MU);
        }
    }

    void World::reset()
    {
        m_objects.clear();

        const OrbitalState start = elementsToState(START_ORBIT, ORBIT_MU);

        GameObject& vessel = m_objects.emplace_back();
        vessel.transform.position = start.position;
        vessel.velocity = start.velocity;
        vessel.previousPosition = start.position;
        vessel.vessel.emplace();
        vessel.transform.scale = 0.5;

        m_initialEnergy = specificEnergy(start, ORBIT_MU);
        m_missionTime = 0.0;
    }

    GameObject& World::vessel()
    {
        return m_objects.front();
    }

    const GameObject& World::vessel() const
    {
        return m_objects.front();
    }

    void World::setBurn(const glm::dvec2& burn)
    {
        vessel().vessel->burn = burn;
    }

    const std::vector<GameObject>& World::objects() const
    {
        return m_objects;
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
        return vessel().vessel->burn != glm::dvec2(0.0);
    }
} // Stellar