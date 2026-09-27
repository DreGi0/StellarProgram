/**
 * @file orbit.h
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

#include <glm/vec3.hpp>

namespace Stellar
{
    struct OrbitalState
    {
        glm::dvec3 position;
        glm::dvec3 velocity;
    };

    glm::dvec3 gravityAcceleration(const glm::dvec3& position, double mu);

    OrbitalState stepExplicitEuler(const OrbitalState& current, double mu, double dt);

    OrbitalState stepSemiImplicitEuler(const OrbitalState& current, double mu, double dt);

    OrbitalState stepVelocityVerlet(const OrbitalState& current, double mu, double dt);

    double specificEnergy(const OrbitalState& state, double mu);

    using StepFunction = OrbitalState (*)(const OrbitalState&, double, double);
} // Stellar
