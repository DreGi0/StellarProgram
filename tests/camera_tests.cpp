/**
 * @file camera_tests.cpp
 * @brief Camera movement and rotation tests.
 * @author DreGi0
 * @date September 26th, 2026
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "graphics/camera.h"
#include "graphics/orbit_camera.h"

TEST_CASE("moveForward moves along the forward direction")
{
    Stellar::Camera camera(glm::dvec3(0.0, 0.0, 5.0));

    camera.moveForward(1.0f);

    const glm::dvec3 pos = camera.getPosition();
    REQUIRE(pos.x == Catch::Approx(0.0f));
    REQUIRE(pos.y == Catch::Approx(0.0f));
    REQUIRE(pos.z == Catch::Approx(4.0f));
}

TEST_CASE("moveRight moves along to the right direction") {
    Stellar::Camera camera(glm::dvec3(0.0, 0.0, 5.0));

    camera.moveRight(1.0f);

    const glm::dvec3 pos = camera.getPosition();
    REQUIRE(pos.x == Catch::Approx(1.0f));
    REQUIRE(pos.y == Catch::Approx(0.0f));
    REQUIRE(pos.z == Catch::Approx(5.0f));
}

TEST_CASE("pitch is limited") {
    Stellar::Camera camera(glm::dvec3(0.0, 0.0, 5.0));

    camera.rotate(0.0f, 1000.0f );

    const float forward = camera.getForward().y;
    REQUIRE(forward == Catch::Approx(glm::sin(glm::radians(89.0f))));
}

TEST_CASE("forward always 1") {
    Stellar::Camera camera(glm::dvec3(0.0, 0.0, 5.0));

    camera.rotate(37.0f, -12.0f);

    REQUIRE(glm::length(camera.getForward())== Catch::Approx(1.0f));
}

TEST_CASE("double precision") {
    Stellar::Camera camera(glm::dvec3(10'000'000, 0, 0));

    camera.moveForward(0.01f);

    REQUIRE(camera.getPosition().z == Catch::Approx(-0.01));
}

TEST_CASE("orbit camera stays at its distance while turning") {
    Stellar::OrbitCamera camera(10.0);

    camera.rotate(73.0f, -31.0f);

    REQUIRE(glm::length(camera.offset()) == Catch::Approx(10.0));
}

TEST_CASE("orbit camera zoom is clamped") {
    Stellar::OrbitCamera camera(10.0);

    camera.zoom(0.0);

    REQUIRE(glm::length(camera.offset()) == Catch::Approx(1.5));
}

TEST_CASE("orbit camera pitch is limited") {
    Stellar::OrbitCamera camera(10.0);

    camera.rotate(0.0f, 1000.0f);

    REQUIRE(camera.offset().z / 10.0 == Catch::Approx(glm::sin(glm::radians(89.0))));
}

TEST_CASE("orbit camera 'up' points away from the planet") {
    Stellar::OrbitCamera camera(10.0);

    camera.setFrame(glm::dvec3(0.0, 10.0, 0.0), glm::dvec3(-1.0, 0.0, 0.0));
    camera.rotate(0.0f, 1000.0f); // Pitch clamped at 89: almost straight above the vessel

    const glm::dvec3 direction = glm::normalize(camera.offset());
    REQUIRE(direction.y == Catch::Approx(glm::sin(glm::radians(89.0))));
}
