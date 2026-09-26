/**
 * @file application.cpp
 * @brief
 * @author DreGi0
 * @date September 25th, 2026
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application.h"
#include "core/paths.h"


// Model geometry
constexpr float CUBE_VERTICES[] = {
     // Position (x, y, z)     |  Color (r, g, b)
     // Front (+Z) - red
     -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // A
      0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // B
      0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // C
     -0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // D

     // Back (-Z) - green
      0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // F
     -0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // E
     -0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // H
      0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // G

     // Right (+X) - blue
      0.5f, -0.5f,  0.5f,       0.0f, 0.0f, 1.0f,   // B
      0.5f, -0.5f, -0.5f,       0.0f, 0.0f, 1.0f,   // F
      0.5f,  0.5f, -0.5f,       0.0f, 0.0f, 1.0f,   // G
      0.5f,  0.5f,  0.5f,       0.0f, 0.0f, 1.0f,   // C

     // Left (-X) - yellow
     -0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 0.0f,   // E
     -0.5f, -0.5f,  0.5f,       1.0f, 1.0f, 0.0f,   // A
     -0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 0.0f,   // D
     -0.5f,  0.5f, -0.5f,       1.0f, 1.0f, 0.0f,   // H

     // Top (+Y) - cyan
     -0.5f,  0.5f,  0.5f,       0.0f, 1.0f, 1.0f,   // D
      0.5f,  0.5f,  0.5f,       0.0f, 1.0f, 1.0f,   // C
      0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,   // G
     -0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,   // H

     // Bottom (-Y) - magenta
     -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,   // E
      0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,   // F
      0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,   // B
     -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,   // A
};

// Index data: 2 triangles per face, 4 vertices per face
constexpr unsigned int CUBE_INDICES[] = {
     0,  1,  2,    2,  3,  0,   // Front
     4,  5,  6,    6,  7,  4,   // Back
     8,  9, 10,   10, 11,  8,   // Right
    12, 13, 14,   14, 15, 12,   // Left
    16, 17, 18,   18, 19, 16,   // Top
    20, 21, 22,   22, 23, 20,   // Bottom
};

constexpr float moveSpeed = 10.0f;
constexpr float mouseSensitivity = 0.1f;

namespace Stellar
{
    Application::Application() :
    m_window(800, 600, "Stellar Program"),
    m_graphicsContext(),
    m_shader(asset_path("shaders/triangle.vert"), asset_path("shaders/triangle.frag")),
    m_cubeMesh(CUBE_VERTICES, 24, CUBE_INDICES, 36),
    m_camera(glm::vec3(0.0f, 0.0f, 5.0f))
    {
        // Viewport adjustment
        int fbWidth = 0;
        int fbHeight = 0;
        m_window.get_framebuffer_size(fbWidth, fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClearColor(0.0f, 0.07f, 0.12f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        glfwSetInputMode(m_window.get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(m_window.get_window(), &m_lastMouseX, &m_lastMouseY);

        m_lastFrameTime = static_cast<float>(glfwGetTime());
    }

    void Application::run()
    {
        while (!m_window.shouldClose()) {
            const auto time = static_cast<float>(glfwGetTime());
            const float deltaTime = time - m_lastFrameTime;
            m_lastFrameTime = time;

            process_input(deltaTime);

            render();

            m_window.swap_buffers();
            Window::poll_events();
        }
    }

    void Application::process_input(float deltaTime)
    {
        if (glfwGetKey(m_window.get_window(), GLFW_KEY_W) == GLFW_PRESS)
        {
            m_camera.move_forward(moveSpeed * deltaTime);
        }

        if (glfwGetKey(m_window.get_window(), GLFW_KEY_D) == GLFW_PRESS)
        {
            m_camera.move_right(moveSpeed * deltaTime);
        }

        if (glfwGetKey(m_window.get_window(), GLFW_KEY_S) == GLFW_PRESS)
        {
            m_camera.move_forward(-moveSpeed * deltaTime);
        }

        if (glfwGetKey(m_window.get_window(), GLFW_KEY_A) == GLFW_PRESS)
        {
            m_camera.move_right(-moveSpeed * deltaTime);
        }

        double mouseX = 0, mouseY = 0;
        glfwGetCursorPos(m_window.get_window(), &mouseX, &mouseY);

        double offsetX = mouseX - m_lastMouseX;
        double offsetY = m_lastMouseY - mouseY;

        m_lastMouseX = mouseX;
        m_lastMouseY = mouseY;

        offsetX = offsetX * mouseSensitivity;
        offsetY = offsetY * mouseSensitivity;

        m_camera.rotate(static_cast<float>(offsetX), static_cast<float>(offsetY));
    }

    void Application::render() const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader.use();

        // COLOR
        m_shader.set_vec3("uColor", 1.0f, 1.0f, 1.0f);

        // MODEL
        constexpr auto modelMatrix = glm::mat4(1.0f);
        m_shader.set_mat4("uModel", glm::value_ptr(modelMatrix));

        // VIEW
        const auto viewMatrix = m_camera.get_view_matrix();
        m_shader.set_mat4("uView", glm::value_ptr(viewMatrix));

        // PROJECTION
        int fbWidth = 0, fbHeight = 0;
        m_window.get_framebuffer_size(fbWidth, fbHeight);

        const float aspectRatio = (fbHeight > 0)
            ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight)
            : 1.0f;

        const auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        m_shader.set_mat4("uProjection", glm::value_ptr(projectionMatrix));

        m_cubeMesh.draw();
    }
} // Stellar