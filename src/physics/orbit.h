/**
 * @file orbit.h
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Stellar
{
    struct OrbitalState
    {
        glm::dvec3 position;
        glm::dvec3 velocity;
    };

    struct OrbitalElements
    {
        double semiMajorAxis; // a: size of the ellipse
        double eccentricity; // e: shape (0 = circle, 0..1 = ellipse)
        double inclination; // i: tilt of the orbit plane, [0, pi]
        double longitudeOfAscendingNode; // ohm: where the orbit crosses the XY plane going up, [0, 2*pi)
        double argumentOfPeriapsis; // w: where the closest point is, measured from the node, [0, 2*pi)
        double trueAnomaly; // ν: where the body is right now, measured from periapsis, [0, 2*pi)
    };

    glm::dvec3 gravityAcceleration(const glm::dvec3& position, double mu);

    OrbitalState stepExplicitEuler(const OrbitalState& current, double mu, double dt);

    OrbitalState stepSemiImplicitEuler(const OrbitalState& current, double mu, double dt);

    OrbitalState stepVelocityVerlet(const OrbitalState& current, double mu, double dt);

    double specificEnergy(const OrbitalState& state, double mu);

    OrbitalElements stateToElements(const OrbitalState& state, double mu);

    glm::dmat4 orbitRotation(const OrbitalElements& elements);

    glm::dmat4 orbitEllipseMatrix(const OrbitalElements& elements);

    OrbitalState elementsToState(const OrbitalElements& elements, double mu);

    OrbitalElements propagateKepler(const OrbitalElements& elements, double mu, double dt);

    OrbitalState stepKepler(const OrbitalState& current, double mu, double dt);

    using StepFunction = OrbitalState (*)(const OrbitalState&, double, double);
} // Stellar
