/**
 * @file primitives_tests.cpp
 * @brief Checks the generated sphere: sizes, radius and index range.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#include <cmath>
#include <cstddef>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "graphics/primitives.h"

TEST_CASE("Unit sphere has the right size, radius 1 and valid indices", "[primitives]")
{
    constexpr int STACKS = 8;
    constexpr int SLICES = 16;
    constexpr std::size_t VERTEX_COUNT = (STACKS + 1) * (SLICES + 1);

    const Stellar::MeshData sphere = Stellar::unitSphere(STACKS, SLICES);

    // Sizes: 6 floats per vertex, 6 indices (two triangles) per cell
    REQUIRE(sphere.vertices.size() == VERTEX_COUNT * 6);
    REQUIRE(sphere.indices.size() == static_cast<std::size_t>(STACKS * SLICES * 6));

    // Every vertex is at distance 1 from the center
    for (std::size_t v = 0; v < sphere.vertices.size(); v += 6)
    {
        const float x = sphere.vertices[v];
        const float y = sphere.vertices[v + 1];
        const float z = sphere.vertices[v + 2];

        REQUIRE(std::sqrt(x * x + y * y + z * z) == Catch::Approx(1.0f));
    }

    // No index points past the last vertex (catches off-by-one errors in the loops)
    for (const unsigned int index : sphere.indices)
    {
        REQUIRE(index < VERTEX_COUNT);
    }
}