/**
 * @file mesh.cpp
 * @brief Mesh class implementation and OpenGL state handling.
 * @author DreGi0
 * @date September 13th, 2026
 */

#include <glad/glad.h>

#include "mesh.h"

namespace Stellar {
    Mesh::Mesh(const float *vertices, const size_t vertexCount) {
        m_vertexCount = vertexCount;

        // Reserve IDs for VAO & VBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        // Pipeline state configuration
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // Data transfer from RAM (CPU) to VRAM (GPU)
        const GLsizeiptr bufferSize = vertexCount * 6 * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);

        // Attribute 0: Position (x, y, z)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
            reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        // Attribute 1: Color (R, G, B) with 3 float offset
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VAO to avoid unintended modifications
        glBindVertexArray(0);
    }

    Mesh::Mesh(Mesh &&other) noexcept :
    m_vertexCount(other.m_vertexCount),
    m_vao(other.m_vao),
    m_vbo(other.m_vbo) {
        // Reset origin object
        other.m_vertexCount = 0;
        other.m_vbo = 0;
        other.m_vao = 0;
    }

    Mesh & Mesh::operator=(Mesh &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        // Release destination (this object) resources
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);

        // Transfer handle properties
        m_vertexCount = other.m_vertexCount;
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;

        // Reset origin object
        other.m_vertexCount = 0;
        other.m_vao = 0;
        other.m_vbo = 0;

        return *this;
    }

    void Mesh::draw() const {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
    }
} // Stellar