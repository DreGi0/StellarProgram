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
    Camera::Camera(const glm::dvec3& position)
    {
        m_position = position;
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(glm::vec3(0.0f), m_forward, m_up);
    }

    void Camera::moveForward(const float distance)
    {
        m_position += m_forward * distance;
    }

    void Camera::moveRight(const float distance)
    {
        m_position += glm::normalize(glm::cross(m_forward, m_up)) * distance;
    }

    void Camera::rotate(const float yawOffset, const float pitchOffset)
    {
        m_yaw += yawOffset;
        m_pitch += pitchOffset;

        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        m_forward = glm::normalize(glm::vec3(glm::cos(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch)), glm::sin(glm::radians(m_pitch)), glm::sin(glm::radians(m_yaw)) * glm::cos(glm::radians(m_pitch))));
    }

    glm::mat4 reversedInfinitePerspective(const float fovY, const float aspect, const float nearPlane)
    {
        const float f = 1.0f / std::tan(fovY / 2.0f);

        glm::mat4 projection(0.0f);
        projection[0][0] = f / aspect;
        projection[1][1] = f;
        projection[2][3] = -1.0f; // w = distance in front of the camera (perspective divide)
        projection[3][2] = nearPlane; // z = nearPlane, so depth = nearPlane / distance

        return projection;
    }
} // Stellar