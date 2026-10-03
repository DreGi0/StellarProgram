/**
 * @file world.h
 * @brief
 * @author DreGi0
 * @date October 2nd, 2026
 */

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "physics/orbit.h"

namespace Stellar
{
    // Orbit (toy units: r = 10, v = 10 -> which is one lap every 2*pi seconds)
    inline constexpr double ORBIT_MU = 1000.0;
    inline constexpr glm::dvec3 CENTRAL_BODY_POSITION(10'000'000.0, 0.0, 0.0);

    // Provisional representation of the object orbiting the body.
    struct Vessel
    {
        OrbitalState state{};
        glm::dvec3 previousPosition = glm::dvec3(0.0);
        glm::dvec2 burn = glm::dvec2(0.0); // x = prograde, y = normal
    };

    class World
    {
        public:
        World();

        void update(double deltaTime);

        void reset();

        Vessel& vessel();
        [[nodiscard]] const Vessel& vessel() const;

        void setIntegrator(int index);
        [[nodiscard]] int integratorIndex() const;

        void setTimeWarp(int index);

        [[nodiscard]] int timeWarpIndex() const;
        [[nodiscard]] double timeWarp() const;
        [[nodiscard]] double physicsWarp() const;

        [[nodiscard]] double missionTime() const;

        [[nodiscard]] double initialEnergy() const;

        [[nodiscard]] bool isOnRails() const;

        [[nodiscard]] bool isBurning() const;

        private:
        Vessel m_vessel;
        double m_initialEnergy = 0.0;
        // Lab tool from step 10 to compare integrators; goes away once Kepler/Verlet are the only path
        int m_integratorIndex = 3; // 0: Explicit Euler, 1:SemiImplicitEuler 2: Velocity Verlet, 3: Kepler
        int m_timeWarpIndex = 0;
        double m_missionTime = 0.0;
    };
} // Stellar
