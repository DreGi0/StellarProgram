/**
 * @file world.h
 * @brief The simulated world: central body, game objects, mission clock and time warp.
 * @author DreGi0
 * @date October 2nd, 2026
 */

#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "game_object.h"
#include "physics/orbit.h"

namespace Stellar
{
    inline constexpr double EARTH_RADIUS = 6'371'000.0;
    inline constexpr double PLANET_SCALE = 3.0;
    inline constexpr double STANDARD_GRAVITY = 9.80665;
    inline constexpr double PLANET_RADIUS = EARTH_RADIUS / PLANET_SCALE;
    inline constexpr double ORBIT_MU = STANDARD_GRAVITY * PLANET_RADIUS * PLANET_RADIUS;
    inline constexpr glm::dvec3 CENTRAL_BODY_POSITION(10'000'000.0, 0.0, 0.0);

    inline constexpr double CONTACT_EPSILON = 1e-6;
    
    /// Position + velocity of an object in the form the orbit functions expect.
    inline OrbitalState orbitalState(const GameObject& object)
    {
        return {
            .position = object.transform.position,
            .velocity = object.velocity
        };
    }

    /**
     * @class World
     * @brief Owns the simulation state and advances it in fixed steps.
     */
    class World
    {
        public:
        /**
         * @brief Creates the world with the vessel on its starting orbit.
         */
        World();

        /**
         * @brief Advances the simulation one fixed step.
         *
         * Engine off: moves every object with the selected integrator, with time warp applied.
         * Engine on: the vessel leaves the rails and steps with Verlet plus thrust, without time warp.
         * @param deltaTime Real seconds in this step (the fixed timestep).
         */
        void update(double deltaTime);

        /**
         * @brief Puts the vessel back on the starting orbit and the mission clock at zero.
         */
        void reset();

        /**
         * @brief The vessel, writable. Always the first object.
         */
        GameObject& vessel();

        /**
         * @brief The vessel, read-only (e.g. for rendering). Always the first object.
         */
        [[nodiscard]] const GameObject& vessel() const;

        /**
         * @brief Every object in the world, the vessel first.
         */
        [[nodiscard]] const std::vector<GameObject>& objects() const;

        /**
         * @brief Sets the vessel's burn command.
         * @param burn x = prograde (+) / retrograde (-), y = normal (+) / antinormal (-).
         */
        void setBurn(const glm::dvec2& burn);

        /**
         * @brief Selects the integrator and resets the orbit, so each one starts from the same state.
         * @param index 0: Explicit Euler, 1: Semi-implicit Euler, 2: Velocity Verlet, 3: Kepler.
         */
        void setIntegrator(int index);

        /**
         * @return Index of the selected integrator (see setIntegrator).
         */
        [[nodiscard]] int integratorIndex() const;

        /**
         * @brief Selects the time warp level.
         * @param index 0: x1, 1: x5, 2: x10, 3: x50, 4: x100, 5: x1000, 6: x10000, 7: x100000.
         */
        void setTimeWarp(int index);

        /**
         * @return Index of the selected time warp level (see setTimeWarp).
         */
        [[nodiscard]] int timeWarpIndex() const;

        /**
         * @brief Time warp actually applied inside each step.
         * @return The selected level when on rails and not burning; 1 otherwise.
         */
        [[nodiscard]] double timeWarp() const;

        /**
         * @brief How many steps per real second to run while burning (sub-stepping).
         * @return The selected level capped at 4 while burning; 1 otherwise.
         */
        [[nodiscard]] double physicsWarp() const;

        /**
         * @return Simulated seconds since the last reset.
         */
        [[nodiscard]] double missionTime() const;

        /**
         * @brief Reference energy used to measure the integrator's drift.
         *
         * Updated after every burn step, because a burn changes the energy on purpose.
         */
        [[nodiscard]] double initialEnergy() const;

        /**
         * @return true when the selected integrator is Kepler (exact for any step size).
         */
        [[nodiscard]] bool isOnRails() const;

        /**
         * @return true while the burn command is not zero.
         */
        [[nodiscard]] bool isBurning() const;

        private:
        std::vector<GameObject> m_objects; // The vessel is always the first one (see reset)
        double m_initialEnergy = 0.0;
        // Lab tool from step 10 to compare integrators; goes away once Kepler/Verlet are the only path
        int m_integratorIndex = 3;
        int m_timeWarpIndex = 0;
        double m_missionTime = 0.0;
    };
} // Stellar
