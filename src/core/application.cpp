/**
 * @file application.cpp
 * @brief Main loop, input handling and scene rendering.
 * @author DreGi0
 * @date September 25th, 2026
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

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

// A circle of radius 1 on the XY plane, drawn as a closed line.
// orbitEllipseMatrix() stretches, shifts and rotates it into the real orbit every frame.
Stellar::Mesh makeUnitCircleMesh()
{
    constexpr int SEGMENTS = 256;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i < SEGMENTS; ++i)
    {
        constexpr float ORBIT_COLOR[] = { 0.35f, 0.65f, 1.0f };
        const float angle = glm::two_pi<float>() * static_cast<float>(i) / SEGMENTS;

        vertices.insert(vertices.end(), {std::cos(angle), std::sin(angle), 0.0f, ORBIT_COLOR[0], ORBIT_COLOR[1], ORBIT_COLOR[2]});
        indices.push_back(i);
    }

    return {vertices.data(), SEGMENTS, indices.data(), indices.size(), GL_LINE_LOOP};
}

constexpr float MOVE_SPEED = 10.0f;
constexpr float MOUSE_SENSITIVITY = 0.1f;

constexpr double FIXED_DT = 1.0 / 60.0;

namespace Stellar
{
    Application::Application() :
    m_window(800, 600, "Stellar Program"),
    m_input(m_window.getHandle()),
    m_graphicsContext(),
    m_debugOverlay(m_window.getHandle()),
    m_shader(assetPath("shaders/triangle.vert"), assetPath("shaders/triangle.frag")),
    m_cubeMesh(CUBE_VERTICES, 24, CUBE_INDICES, 36),
    m_orbitMesh(makeUnitCircleMesh()),
    m_camera(glm::dvec3(10'000'000.0, 0.0f, 30.0f))
    {
        // Viewport adjustment
        int fbWidth = 0;
        int fbHeight = 0;
        m_window.getFramebufferSize(fbWidth, fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        glClearColor(0.0f, 0.07f, 0.12f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        m_lastFrameTime = glfwGetTime();
    }

    void Application::run()
    {
        while (!m_window.shouldClose()) {
            const auto time = glfwGetTime();
            double frameTime = time - m_lastFrameTime;
            m_lastFrameTime = time;

            m_input.update();

            processInput(static_cast<float>(frameTime));

            frameTime = glm::min(frameTime, 0.25);
            m_accumulatedTime += frameTime * m_world.physicsWarp();

            while (m_accumulatedTime >= FIXED_DT)
            {
                m_world.update(FIXED_DT);
                m_accumulatedTime -= FIXED_DT;
            }

            const double alpha = m_accumulatedTime / FIXED_DT;

            render(static_cast<float>(alpha));

            m_window.swapBuffers();
            Window::pollEvents();
        }
    }

    void Application::processInput(const float deltaTime)
    {
        if (m_input.wasPressed(Action::ToggleCursor))
        {
            m_input.setCursorCaptured(!m_input.isCursorCaptured());
        }

        m_world.vessel().burn = glm::dvec2(
            m_input.axis(Action::BurnPrograde),
            m_input.axis(Action::BurnNormal));

        if (!m_input.isCursorCaptured())
        {
            return;
        }

        // CAMERA
        const float step = MOVE_SPEED * deltaTime;

        m_camera.moveForward(step * static_cast<float>(m_input.axis(Action::MoveForward)));
        m_camera.moveRight(step * static_cast<float>(m_input.axis(Action::MoveRight)));

        const glm::dvec2 mouse = m_input.mouseDelta() * static_cast<double>(MOUSE_SENSITIVITY);

        m_camera.rotate(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
    }

    void Application::render(const float alpha)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shader.use();

        // COLOR
        m_shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);

        // VIEW
        const auto viewMatrix = m_camera.getViewMatrix();
        m_shader.setMat4("uView", glm::value_ptr(viewMatrix));

        // PROJECTION
        int fbWidth = 0, fbHeight = 0;
        m_window.getFramebufferSize(fbWidth, fbHeight);

        const float aspectRatio = (fbHeight > 0)
            ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight)
            : 1.0f;

        const auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
        m_shader.setMat4("uProjection", glm::value_ptr(projectionMatrix));

        // MODEL - "Earth"
        const glm::dvec3 centralRelativeToCam = CENTRAL_BODY_POSITION - m_camera.getPosition();
        const auto centralModel = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(centralRelativeToCam)), glm::vec3(2.0f));

        m_shader.setMat4("uModel", glm::value_ptr(centralModel));
        m_cubeMesh.draw();

        // MODEL - "Earth's Satellite"
        // With time warp each physics step jumps a big arc of the orbit, and blending two points
        // of an ellipse in a straight line cuts inside it, so interpolation only at x1
        const Vessel& vessel = m_world.vessel();

        const double interpolation = m_world.timeWarp() > 1.0 ? 1.0 : static_cast<double>(alpha);
        const glm::dvec3 orbitPosition = glm::mix(vessel.previousPosition, vessel.state.position, interpolation);
        const glm::dvec3 orbiterRelativeToCam = CENTRAL_BODY_POSITION + orbitPosition - m_camera.getPosition();
        const auto orbiterModel = glm::translate(glm::mat4(1.0f), glm::vec3(orbiterRelativeToCam));

        m_shader.setMat4("uModel", glm::value_ptr(orbiterModel));
        m_cubeMesh.draw();

        // MODEL - Orbit line (ellipses only)
        const OrbitalElements elements = stateToElements(vessel.state, ORBIT_MU);

        if (elements.eccentricity < 1.0)
        {
            const glm::dmat4 orbitModel = glm::translate(glm::dmat4(1.0), centralRelativeToCam) * orbitEllipseMatrix(elements);
            const glm::mat4 orbitModelF(orbitModel);

            m_shader.setMat4("uModel", glm::value_ptr(orbitModelF));
            m_orbitMesh.draw();
        }

        // DEBUG OVERLAY
        m_debugOverlay.draw(m_world, m_camera);
    }
} // Stellar