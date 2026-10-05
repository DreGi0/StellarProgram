/**
 * @file orbit_camera.cpp
 * @brief OrbitCamera implementation.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#include "orbit_camera.h"

#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

namespace
{
    constexpr double MIN_DISTANCE = 5.0; // The vessel is 2 m in radius
    constexpr double MAX_DISTANCE = 10'000'000.0; // 10 000 km: the whole planet fits on screen
}

namespace Stellar
{
    OrbitCamera::OrbitCamera(const double distance) :
    m_distance(glm::clamp(distance, MIN_DISTANCE, MAX_DISTANCE))
    {
    }

    void OrbitCamera::setFrame(const glm::dvec3& position, const glm::dvec3& velocity)
    {

        const glm::dvec3 up = glm::normalize(position);
        const glm::dvec3 forward = glm::cross(glm::cross(position, velocity), up);

        if (glm::length(forward) < 1e-9)
        {
            return;
        }

        m_up = up;
        m_forward = glm::normalize(forward);
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

        const glm::dvec3 side = glm::cross(m_up, m_forward);

        return m_distance * (
            glm::cos(pitch) * glm::cos(yaw) * m_forward +
            glm::cos(pitch) * glm::sin(yaw) * side +
            glm::sin(pitch) * m_up);
    }

    glm::mat4 OrbitCamera::getViewMatrix() const
    {
        return glm::lookAt(glm::vec3(0.0f), glm::vec3(-offset()), glm::vec3(m_up));
    }
} // Stellar