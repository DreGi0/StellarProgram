/**
 * @file orbit.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include "orbit.h"

#include <glm/geometric.hpp>

namespace Stellar
{
    glm::dvec3 gravityAcceleration(const glm::dvec3& position, const double mu)
    {
        const double r = glm::length(position);

        return -mu * position / (r * r * r);
    }

    OrbitalState stepExplicitEuler(const OrbitalState& current, const double mu, const double dt)
    {
        OrbitalState next{};

        const glm::dvec3 a = gravityAcceleration(current.position, mu);

        next.position = current.position + current.velocity * dt;
        next.velocity = current.velocity + a * dt;

        return next;
    }

    OrbitalState stepSemiImplicitEuler(const OrbitalState& current, const double mu, const double dt)
    {
        OrbitalState next{};

        const glm::dvec3 a = gravityAcceleration(current.position, mu);

        next.velocity = current.velocity + a * dt;
        next.position = current.position + next.velocity * dt;

        return next;
    }

    OrbitalState stepVelocityVerlet(const OrbitalState& current, const double mu, const double dt)
    {
        OrbitalState next{};

        const glm::dvec3 aCurrent = gravityAcceleration(current.position, mu);
        next.position = current.position + current.velocity * dt + 0.5 * aCurrent * (dt * dt);

        const glm::dvec3 aNext = gravityAcceleration(next.position, mu);
        next.velocity = current.velocity + 0.5 * (aCurrent + aNext) * dt;

        return next;
    }

    double specificEnergy(const OrbitalState& state, const double mu)
    {
        const double v2 = glm::dot(state.velocity, state.velocity);
        const double r = glm::length(state.position);

        return 0.5 * v2 - (mu / r);
    }
} // Stellar
