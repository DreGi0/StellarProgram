/**
 * @file orbit_camera.cpp
 * @brief OrbitCamera implementation.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#include "orbit_camera.h"

#include <glm/common.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace
{
    constexpr double MIN_DISTANCE = 1.5;
    constexpr double MAX_DISTANCE = 500.0;
}

namespace Stellar
{
    OrbitCamera::OrbitCamera(const double distance) :
    m_distance(glm::clamp(distance, MIN_DISTANCE, MAX_DISTANCE))
    {
    }

    void OrbitCamera::rotate(const float yawOffset, const float pitchOffset)
    {
        m_yaw += yawOffset;
        m_pitch = glm::clamp(m_pitch + pitchOffset, -89.0f, 89.0f);
    }

    void OrbitCamera::zoom(const double factor)
    {
        m_distance = glm::clamp(m_distance * factor, MIN_DISTANCE, MAX_DISTANCE);
    }

    glm::dvec3 OrbitCamera::offset() const
    {
        const double yaw = glm::radians(static_cast<double>(m_yaw));
        const double pitch = glm::radians(static_cast<double>(m_pitch));

        return m_distance * glm::dvec3(
            glm::cos(pitch) * glm::cos(yaw),
            glm::cos(pitch) * glm::sin(yaw),
            glm::sin(pitch));
    }

    glm::mat4 OrbitCamera::getViewMatrix() const
    {
        // The camera sits at the origin and looks back at the target, which is at -offset
        return glm::lookAt(glm::vec3(0.0f), glm::vec3(-offset()), glm::vec3(0.0f, 0.0f, 1.0f));
    }
} // Stellar