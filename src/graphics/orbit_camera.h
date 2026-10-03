/**
 * @file orbit_camera.h
 * @brief Camera that circles around a target (the vessel), KSP style.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace Stellar
{
    /**
     * @class OrbitCamera
     * @brief Looks at a target from a distance and turns around it with yaw/pitch.
     *
     * It does not store the target: the caller adds offset() to wherever the target is this frame,
     * so the camera follows it for free. Z is "up", like the orbital frame.
     */
    class OrbitCamera
    {
        public:
        /**
         * @param distance Starting distance to the target.
         */
        explicit OrbitCamera(double distance);

        /**
         * @brief Aligns the camera with the vessel's orbit: "up" points away from the planet and
         * yaw 0 looks along the direction of travel. Call it every frame before offset() / getViewMatrix().
         * @param position Vessel position relative to the planet.
         * @param velocity Vessel velocity.
         */
        void setFrame(const glm::dvec3& position, const glm::dvec3& velocity);

        /**
         * @brief Turns around the target. Pitch is clamped to +/-89 degrees so it never flips over the top.
         * @param yawOffset Degrees around the Z axis.
         * @param pitchOffset Degrees up/down.
         */
        void rotate(float yawOffset, float pitchOffset);

        /**
         * @brief Multiplies the distance (below 1 gets closer, above 1 moves away), kept within limits.
         * @param factor Zoom factor.
         */
        void zoom(double factor);

        /**
         * @brief Where the camera is, relative to the target.
         * @return Vector from the target to the camera; its length is the distance.
         */
        [[nodiscard]] glm::dvec3 offset() const;


        /**
         * @brief View matrix with rotation only (camera-relative rendering), looking at the target.
         */
        [[nodiscard]] glm::mat4 getViewMatrix() const;

        private:
        double m_distance;
        float m_yaw = 180.0f;  ///< Degrees around "up", measured from "forward" (180 = behind the vessel)
        float m_pitch = 20.0f; ///< Degrees above the horizontal, clamped to [-89, 89]

        glm::dvec3 m_up = glm::dvec3(0.0, 0.0, 1.0);      ///< Away from the planet
        glm::dvec3 m_forward = glm::dvec3(1.0, 0.0, 0.0); ///< Direction of travel, flattened to the horizontal
    };
} // Stellar
