/**
 * @file mesh.cpp
 * @brief Mesh class implementation and OpenGL state handling.
 * @author DreGi0
 * @date September 13th, 2026
 */

#include <glad/glad.h>

#include "mesh.h"

namespace Stellar {
    Mesh::Mesh(const float* vertices, size_t vertexCount, const unsigned int* indices, size_t indexCount)
    {
        m_indexCount = indexCount;

        // Reserve IDs for VAO, VBO & EBO
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glGenBuffers(1, &m_ebo);

        // Pipeline state configuration
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        // Data transfer from RAM (CPU) to VRAM (GPU)
        const GLsizeiptr vertexBufferSize = vertexCount * 6 * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, vertices, GL_STATIC_DRAW);

        const GLsizeiptr indexBufferSize = m_indexCount * sizeof(unsigned int);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, indices, GL_STATIC_DRAW);

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
    m_indexCount(other.m_indexCount),
    m_vao(other.m_vao),
    m_vbo(other.m_vbo),
    m_ebo(other.m_ebo) {
        // Reset origin object
        other.m_indexCount = 0;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
    }

    Mesh & Mesh::operator=(Mesh &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        // Release destination (this object) resources
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);

        // Transfer handle properties
        m_indexCount = other.m_indexCount;
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;

        // Reset origin object
        other.m_indexCount = 0;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;

        return *this;
    }

    void Mesh::draw() const {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    }

    Mesh::~Mesh() {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }
} // Stellar