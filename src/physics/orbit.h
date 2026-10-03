/**
 * @file orbit.h
 * @brief Orbital mechanics around one central body: integrators, orbital elements and Kepler propagation.
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Stellar
{
    /**
     * @struct OrbitalState
     * @brief Where the body is and how it moves, relative to the central body.
     */
    struct OrbitalState
    {
        glm::dvec3 position; ///< Position relative to the central body
        glm::dvec3 velocity; ///< Velocity relative to the central body
    };

    /**
     * @struct OrbitalElements
     * @brief The same orbit described as a shape plus a place on it, instead of position + velocity.
     */
    struct OrbitalElements
    {
        double semiMajorAxis; ///< a: size of the ellipse
        double eccentricity; ///< e: shape (0 = circle, 0..1 = ellipse)
        double inclination; ///< i: tilt of the orbit plane, [0, pi]
        double longitudeOfAscendingNode; ///< Omega: where the orbit crosses the XY plane going up, [0, 2*pi)
        double argumentOfPeriapsis; ///< w: where the closest point is, measured from the node, [0, 2*pi)
        double trueAnomaly; ///< nu: where the body is right now, measured from periapsis, [0, 2*pi)
    };

    /**
     * @brief Gravity pull at a point: always toward the central body, weaker with distance squared.
     * @param position Position relative to the central body.
     * @param mu Gravitational parameter of the central body.
     * @return Acceleration vector.
     */
    glm::dvec3 gravityAcceleration(const glm::dvec3& position, double mu);

    /**
     * @brief One step of explicit Euler: moves with the old velocity, then updates it.
     * Simplest method; gains energy every step, so orbits spiral outward. Kept to compare.
     */
    OrbitalState stepExplicitEuler(const OrbitalState& current, double mu, double dt);

    /**
     * @brief One step of semi-implicit Euler: updates the velocity first, then moves with it.
     * Same cost as explicit Euler but the energy stays bounded.
     */
    OrbitalState stepSemiImplicitEuler(const OrbitalState& current, double mu, double dt);

    /**
     * @brief One step of velocity Verlet: averages the acceleration at the start and the end of the step.
     * Much more accurate than Euler for the same step size.
     */
    OrbitalState stepVelocityVerlet(const OrbitalState& current, double mu, double dt);

    /**
     * @brief Velocity Verlet with an extra constant acceleration from the engine.
     * @param thrust Engine acceleration (thrust / mass), already pointed in its direction.
     */
    OrbitalState stepVelocityVerletWithThrust(const OrbitalState& current, double mu, double dt, const glm::dvec3& thrust);

    /**
     * @brief Unit vector to point the engine at.
     * @param prograde +1 forward / -1 backward along the velocity (raises / lowers the orbit).
     * @param normal +1 / -1 perpendicular to the orbit plane (tilts the orbit).
     * @return Normalized direction, or a zero vector if both inputs are 0.
     */
    glm::dvec3 burnDirection(const OrbitalState& state, double prograde, double normal);

    /**
     * @brief Energy per unit mass (kinetic + potential). Constant for a perfect orbit,
     * so how much it changes measures an integrator's error.
     * @return Negative for closed orbits (ellipses), 0 or positive for escape.
     */
    double specificEnergy(const OrbitalState& state, double mu);

    /**
     * @brief Converts position + velocity into orbital elements (shape + place on the orbit).
     */
    OrbitalElements stateToElements(const OrbitalState& state, double mu);

    /**
     * @brief Rotation that takes the flat orbit (on XY, periapsis on +X) to its real orientation:
     * Omega around Z, then i around X, then w around Z.
     */
    glm::dmat4 orbitRotation(const OrbitalElements& elements);

    /**
     * @brief Model matrix that turns a unit circle on XY into the orbit ellipse,
     * with the central body at the origin. Used to draw the orbit line.
     */
    glm::dmat4 orbitEllipseMatrix(const OrbitalElements& elements);

    /**
     * @brief Converts orbital elements back into position + velocity.
     */
    OrbitalState elementsToState(const OrbitalElements& elements, double mu);

    /**
     * @brief Moves the body along its orbit by dt using Kepler's equation.
     * Only the true anomaly changes; exact for any dt, so it works with any time warp.
     */
    OrbitalElements propagateKepler(const OrbitalElements& elements, double mu, double dt);

    /**
     * @brief Kepler propagation with the same signature as the integrators ("on rails"):
     *        state -> elements -> move along the orbit -> state.
     */
    OrbitalState stepKepler(const OrbitalState& current, double mu, double dt);

    /// Signature shared by all the step functions, so World can pick one at runtime.
    using StepFunction = OrbitalState (*)(const OrbitalState&, double, double);
} // Stellar
