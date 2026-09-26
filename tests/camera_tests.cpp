/**
 * @file camera_tests.cpp
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "graphics/camera.h"

TEST_CASE("moveForward moves along the forward direction") {
    Stellar::Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    camera.moveForward(1.0f);

    const glm::vec3 pos = camera.getPosition();
    REQUIRE(pos.x == Catch::Approx(0.0f));
    REQUIRE(pos.y == Catch::Approx(0.0f));
    REQUIRE(pos.z == Catch::Approx(4.0f));
}

TEST_CASE("moveRight moves along to the right direction") {
    Stellar::Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    camera.moveRight(1.0f);

    const glm::vec3 pos = camera.getPosition();
    REQUIRE(pos.x == Catch::Approx(1.0f));
    REQUIRE(pos.y == Catch::Approx(0.0f));
    REQUIRE(pos.z == Catch::Approx(5.0f));
}

TEST_CASE("pitch is limited") {
    Stellar::Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    camera.rotate(0.0f, 1000.0f );

    const float forward = camera.getForward().y;
    REQUIRE(forward == Catch::Approx(glm::sin(glm::radians(89.0f))));
}

TEST_CASE("forward always 1") {
    Stellar::Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

    camera.rotate(37.0f, -12.0f);

    REQUIRE(glm::length(camera.getForward())== Catch::Approx(1.0f));
}