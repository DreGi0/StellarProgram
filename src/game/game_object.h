/**
 * @file game_object.h
 * @brief Plain data for anything that exists in the world: transform, motion and optional parts.
 * @author DreGi0
 * @date October 4th, 2026
 */

#pragma once

#include <optional>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Stellar
{
    /**
     * @struct VesselData
     * @brief What makes a GameObject a vessel: the data only controllable ships have.
     */
    struct VesselData
    {
        glm::dvec2 burn = glm::dvec2(0.0); ///< Burn command: x = prograde (+) / retrograde (-), y = normal (+) / antinormal (-)
    };

    /**
     * @struct Transform
     * @brief Where an object is, how it is rotated and how big it is.
     */
    struct Transform
    {
        glm::dvec3 position = glm::dvec3(0.0); ///< Relative to the central body (the render adds CENTRAL_BODY_POSITION)
        glm::dquat rotation = glm::dquat(1.0, 0.0, 0.0, 0.0); ///< Identity (w, x, y, z); unused for now
        double scale = 1.0; ///< Uniform scale
    };

    /**
     * @struct GameObject
     * @brief Something that exists in the world and moves with the simulation.
     *
     * Plain data with no behavior: World decides what happens to each object
     * based on which optional parts it has (e.g. vessel), not on its type.
     */
    struct GameObject
    {
        Transform transform;
        glm::dvec3 velocity = glm::dvec3(0.0); ///< Relative to the central body, world units per second
        glm::dvec3 previousPosition = glm::dvec3(0.0); ///< Position before the last step, for render interpolation
        std::optional<VesselData> vessel; ///< Set only if this object is a vessel
    };
} // Stellar

