/**
 * @file orbit.h
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

namespace Stellar
{
    static glm::dvec3 gravityAcceleration(const glm::dvec3& position, double mu)
    {
        const double r  = glm::length(position);

        return -mu * position / (r * r * r);
    }
} // Stellar
