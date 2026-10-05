/**
 * @file camera.h
 * @brief Free-look camera (WASD + mouse), used as the debug camera.
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Stellar
{
    /**
     * @class Camera
     * @brief Free-look camera with a double-precision position and yaw/pitch orientation.
     *
     * Rendering is camera-relative: the view matrix has rotation only, and every object
     * is drawn at (object position - camera position) to keep float precision far from the origin.
     */
    class Camera
    {
        public:
        /**
         * @param position Starting position in world units.
         */
        explicit Camera(const glm::dvec3& position);

        /**
         * @brief View matrix with rotation only (the camera sits at the origin).
         */
        [[nodiscard]] glm::mat4 getViewMatrix() const;

        /**
         * @brief Moves along the look direction (negative moves back).
         */
        void moveForward(float distance);

        /**
         * @brief Moves sideways, perpendicular to the look direction (negative moves left).
         */
        void moveRight(float distance);

        /**
         * @brief Turns the camera. Pitch is clamped to +/-89 degrees so it never flips over.
         * @param yawOffset Degrees to turn left/right.
         * @param pitchOffset Degrees to turn up/down.
         */
        void rotate(float yawOffset, float pitchOffset);

        /// Position in world units (double precision).
        [[nodiscard]] glm::dvec3 getPosition() const { return m_position; }

        /// Unit vector the camera looks along.
        [[nodiscard]] glm::vec3 getForward() const { return m_forward; }


        private:
        glm::dvec3 m_position = glm::dvec3(0.0, 0.0, 3.0);
        glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
        float m_yaw = -90.0f;  ///< Degrees around the up axis; -90 looks at -Z
        float m_pitch = 0.0f;  ///< Degrees up/down, clamped to [-89, 89]
    };

    /**
     * @brief Perspective projection with reversed depth and no far plane.
     *
     * Depth is nearPlane / distance: 1 at the near plane, tending to 0 at infinity.
     * Needs glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE) and glDepthFunc(GL_GREATER).
     * @param fovY Vertical field of view, in radians.
     * @param aspect Width / height.
     * @param nearPlane Distance to the near plane; nothing closer is drawn.
     */
    glm::mat4 reversedInfinitePerspective(float fovY, float aspect, float nearPlane);
} // Stellar
