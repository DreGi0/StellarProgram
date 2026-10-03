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
        float m_yaw = -90.0f;  ///< Degrees around Z, measured from +X
        float m_pitch = 20.0f; ///< Degrees above the XY plane, clamped to [-89, 89]
    };
} // Stellar
