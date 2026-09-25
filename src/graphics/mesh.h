/**
 * @file mesh.h
 * @brief Mesh class definition for OpenGL geometry handling.
 * @author DreGi0
 * @date September 13th, 2026
 */

#pragma once

#include <cstddef>
#include <GL/gl.h>

namespace Stellar {
    /**
     * @class Mesh
     * @brief Encapsulates a Vertex Array Object (VAO), Vertex Buffer Object (VBO) and Element Buffer Object (EBO).
     */
    class Mesh {
    public:
        /**
         * @brief Constructs a new Mesh Object and initialize buffers on the GPU.
         * @param vertices Pointer vertex float data array (Position + Color).
         * @param vertexCount Total number of vertices to process.
         * @param indices Pointer to index data array (unsigned int).
         * @param indexCount Total number of indices to draw.
         */
        Mesh(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount);

        // Disable copy to avoid duplicity of OpenGL handles
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        /**
         * @brief Movement constructor. Transfer buffer properties.
         * @param other Instance where the resources are moving from.
         */
        Mesh(Mesh&& other) noexcept;

        /**
         * @brief Movement assignation operator.
         * @param other  Instance where the resources are moving from.
         * @return Reference to this instance.
         */
        Mesh& operator=(Mesh&& other) noexcept;

        /**
         * @brief Mesh rendering using OpenGL draw functions.
         */
        void draw() const;

        /**
         * Destructor. Free up assigned hardware resources (VAO, VBO & EBO)
         */
        ~Mesh();
    private:
        GLsizei m_indexCount = 0;
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLuint m_ebo = 0;
    };
} // Stellar
