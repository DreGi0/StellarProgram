/**
 * @file camera.cpp
 * @brief Camera class implementation for freelook camera
 * @author DreGi0
 * @date September 25th, 2026
 */

#include "camera.h"

#include <glm/ext/matrix_transform.hpp>

namespace Stellar
{
    Camera::Camera(const glm::vec3 position)
    {
        m_position = position;
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(m_position,  m_position + m_forward, m_up);
    }

    void Camera::moveForward(const float distance)
    {
        m_position += m_forward * distance;
    }

    void Camera::moveRight(const float distance)
    {
        m_position += glm::normalize(glm::cross(m_forward, m_up)) * distance;
    }

    void Camera::rotate(float yawOffset, float pitchOffset)
    {
        m_yaw += yawOffset;
        m_pitch += pitchOffset;

        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        m_forward = glm::normalize(glm::vec3(glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)), glm::sin(glm::radians(m_pitch)), glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))));
    }
} // Stellar