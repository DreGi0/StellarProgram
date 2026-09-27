/**
 * @file orbit.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include "orbit.h"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>

namespace
{
    constexpr double ELEMENTS_EPSILON = 1e-10;

    double angleAround(const glm::dvec3& from, const glm::dvec3& to, const glm::dvec3& axis)
    {
        const double angle = std::atan2(glm::dot(glm::cross(from, to), axis), glm::dot(from, to));

        return angle < 0.0 ? angle + glm::two_pi<double>() : angle;
    }
}

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

    OrbitalElements stateToElements(const OrbitalState& state, const double mu)
    {
        const glm::dvec3& r = state.position;
        const glm::dvec3& v = state.velocity;

        const double rLength = glm::length(r);

        // Angular momentum
        const glm::dvec3 h = glm::cross(r, v);
        const glm::dvec3 hDir = glm::normalize(h);

        // Node vector
        const glm::dvec3 n = glm::cross(glm::dvec3(0.0, 0.0, 1.0), h);
        const double nLength = glm::length(n);

        // Eccentricity vector
        const glm::dvec3 eVec = glm::cross(v, h) / mu - r / rLength;
        const double e = glm::length(eVec);

        // Fallbacks
        const glm::dvec3 nodeDir = nLength > ELEMENTS_EPSILON ? n / nLength : glm::dvec3(1.0, 0.0, 0.0);
        const glm::dvec3 periapsisDir = e > ELEMENTS_EPSILON ? eVec / e : nodeDir;

        OrbitalElements elements{};

        elements.semiMajorAxis = -mu / (2.0 * specificEnergy(state, mu));
        elements.eccentricity = e;
        elements.inclination = std::atan2(glm::length(glm::dvec3(h.x, h.y, 0.0)), h.z);
        elements.longitudeOfAscendingNode = angleAround(glm::dvec3(1.0, 0.0, 0.0), nodeDir, glm::dvec3(0.0, 0.0, 1.0));
        elements.argumentOfPeriapsis = angleAround(nodeDir, periapsisDir, hDir);
        elements.trueAnomaly = angleAround(periapsisDir, r, hDir);

        return elements;
    }
} // Stellar
