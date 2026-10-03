/**
 * @file primitives.cpp
 * @brief Generation of the circle and sphere vertex/index data.
 * @author DreGi0
 * @date October 3rd, 2026
 */

#include "primitives.h"

#include <cmath>
#include <glm/gtc/constants.hpp>

namespace Stellar
{
    MeshData unitCircle(const int segments)
    {
        constexpr float ORBIT_COLOR[] = { 0.35f, 0.65f, 1.0f};

        MeshData data;

        for (int i = 0; i < segments; i++)
        {
            const float angle = glm::two_pi<float>() * static_cast<float>(i) / static_cast<float>(segments);

            data.vertices.insert(
                data.vertices.end(), {
                    std::cos(angle),
                    std::sin(angle),
                    0.0f,
                    ORBIT_COLOR[0],
                    ORBIT_COLOR[1],
                    ORBIT_COLOR[2],
                }
            );
            data.indices.push_back(static_cast<unsigned int>(i));
        }

        return data;
    }

    MeshData unitSphere(const int stacks, const int slices)
    {
        MeshData data;

        // VERTICES
        for (int i = 0; i <= stacks; i++)
        {
            const float phi = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(stacks);

            for (int j = 0; j <= slices; j++)
            {
                const float theta = glm::two_pi<float>() * static_cast<float>(j) / static_cast<float>(slices);

                const float x = std::sin(phi) * std::cos(theta);
                const float y = std::sin(phi) * std::sin(theta);
                const float z = std::cos(phi);

                data.vertices.insert(data.vertices.end(), { x, y, z, 1.0f, 1.0f, 1.0f });
            }
        }

        // INDICES
        const auto ring = static_cast<unsigned int>(slices + 1);

        for (int i = 0; i < stacks; i++)
        {
            for (int j = 0; j < slices; j++)
            {
                const unsigned int a = static_cast<unsigned int>(i) * ring + static_cast<unsigned int>(j);
                const unsigned int b = a + ring;

                data.indices.insert(data.indices.end(), { a, b, a + 1, a + 1, b, b + 1 });
            }
        }

        return data;
    }
} // Stellar
