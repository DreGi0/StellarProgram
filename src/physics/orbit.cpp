/**
 * @file orbit.cpp
 * @brief Implementation of the orbital mechanics functions.
 * @author DreGi0
 * @date September 26th, 2026
 */

#include "orbit.h"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
    constexpr double ELEMENTS_EPSILON = 1e-10;

    double angleAround(const glm::dvec3& from, const glm::dvec3& to, const glm::dvec3& axis)
    {
        const double angle = std::atan2(glm::dot(glm::cross(from, to), axis), glm::dot(from, to));

        const double wrapped = angle < 0.0 ? angle + glm::two_pi<double>() : angle;

        return wrapped < glm::two_pi<double>() ? wrapped : 0.0;
    }

    double trueToMeanAnomaly(const double nu, const double e)
    {
        const double E = 2.0 * std::atan2(std::sqrt(1.0 - e) * std::sin(nu / 2.0), std::sqrt(1.0 + e) * std::cos(nu / 2.0));

        return E - e * std::sin(E);
    }

    double meanToTrueAnomaly(const double M, const double e)
    {
        double E = e < 0.8 ? M : glm::pi<double>();

        for (int i = 0; i < 20; ++i)
        {
            const double correction = (E - e * std::sin(E) - M) / (1.0 - e * std::cos(E));
            E -= correction;

            if (std::abs(correction) < 1e-14)
            {
                break;
            }
        }

        const double nu = 2.0 * std::atan2(std::sqrt(1.0 + e) * std::sin(E / 2.0), std::sqrt(1.0 - e) * std::cos(E / 2.0));

        return nu < 0.0 ? nu + glm::two_pi<double>() : nu;
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
        return stepVelocityVerletWithThrust(current, mu, dt, glm::dvec3(0.0));
    }

    OrbitalState stepVelocityVerletWithThrust(const OrbitalState& current, double mu, double dt,
        const glm::dvec3& thrust)
    {

        OrbitalState next{};

        const glm::dvec3 aCurrent = gravityAcceleration(current.position, mu) + thrust;
        next.position = current.position + current.velocity * dt + 0.5 * aCurrent * (dt * dt);

        const glm::dvec3 aNext = gravityAcceleration(next.position, mu) + thrust;
        next.velocity = current.velocity + 0.5 * (aCurrent + aNext) * dt;

        return next;
    }

    glm::dvec3 burnDirection(const OrbitalState& state, double prograde, double normal)
    {
        const glm::dvec3 progradeDir = glm::normalize(state.velocity);
        const glm::dvec3 normalDir = glm::normalize(glm::cross(state.position, state.velocity));

        const glm::dvec3 direction = prograde * progradeDir + normal * normalDir;
        const double length = glm::length(direction);

        return length > 0.0 ? direction / length : glm::dvec3(0.0);
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

    glm::dmat4 orbitRotation(const OrbitalElements& elements)
    {
        glm::dmat4 rotation(1.0);
        rotation = glm::rotate(rotation, elements.longitudeOfAscendingNode, glm::dvec3(0.0, 0.0, 1.0));
        rotation = glm::rotate(rotation, elements.inclination, glm::dvec3(1.0, 0.0, 0.0));
        rotation = glm::rotate(rotation, elements.argumentOfPeriapsis, glm::dvec3(0.0, 0.0, 1.0));

        return rotation;
    }

    glm::dmat4 orbitEllipseMatrix(const OrbitalElements& elements)
    {
        const double a = elements.semiMajorAxis;
        const double e = elements.eccentricity;
        const double b = a * std::sqrt(1.0 - e * e); // half of the ellipse's short side

        // Stretch the circle into an ellipse, make the "planet" be the center,
        // then rotate it into place
        glm::dmat4 matrix = orbitRotation(elements);
        matrix = glm::translate(matrix, glm::dvec3(-a * e, 0.0, 0.0));
        matrix = glm::scale(matrix, glm::dvec3(a, b, 1.0));

        return matrix;
    }

    OrbitalState elementsToState(const OrbitalElements& elements, double mu)
    {
        const double a = elements.semiMajorAxis;
        const double e = elements.eccentricity;
        const double nu = elements.trueAnomaly;

        // Semi-latus rectum
        const double p = a * (1.0 - e * e);
        const double r = p / (1.0 + e * glm::cos(nu));

        // Perifocal frame
        const glm::dvec3 positionFlat(r * glm::cos(nu), r * glm::sin(nu), 0.0);
        const glm::dvec3 velocityFlat = glm::sqrt(mu / p) * glm::dvec3(-glm::sin(nu), e + glm::cos(nu), 0.0);

        // Rotate the flat ellipse into place
        const glm::dmat4 rotation = orbitRotation(elements);

        // w = 0: directions only, no translation
        return {
            .position = glm::dvec3(rotation * glm::dvec4(positionFlat, 0.0)),
            .velocity = glm::dvec3(rotation * glm::dvec4(velocityFlat, 0.0)),
        };
    }

    OrbitalElements propagateKepler(const OrbitalElements& elements, double mu, double dt)
    {
        const double a = elements.semiMajorAxis;
        const double e = elements.eccentricity;

        const double n = std::sqrt(mu / (a * a * a));

        const double M0 = trueToMeanAnomaly(elements.trueAnomaly, e);
        const double M = std::fmod(M0 + n * dt, glm::two_pi<double>());

        OrbitalElements next = elements;
        next.trueAnomaly = meanToTrueAnomaly(M, e);

        return next;
    }

    OrbitalState stepKepler(const OrbitalState& current, const double mu, const double dt)
    {
        return elementsToState(propagateKepler(stateToElements(current, mu), mu, dt), mu);
    }

    double periapsisRadius(const OrbitalState& state, const double mu)
    {
        const glm::dvec3 h = glm::cross(state.position, state.velocity);
        const glm::dvec3 eVec = glm::cross(state.velocity, h) / mu - glm::normalize(state.position);

        return glm::dot(h, h) / (mu * (1.0 + glm::length(eVec)));
    }
} // Stellar
