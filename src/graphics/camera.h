/**
 * @file camera.h
 * @brief Camera class definition for freelook camera
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace Stellar
{
    class Camera
    {
        private:
            glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 3.0f);
            glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
            float m_yaw = -90.0f;
            float m_pitch = 0.0f;

        public:
            explicit Camera(glm::vec3 position);

            [[nodiscard]] glm::mat4 get_view_matrix() const;

            void move_forward(float distance);

            void move_right(float distance);

            void rotate(float yawOffset, float pitchOffset);
    };
} // Stellar
