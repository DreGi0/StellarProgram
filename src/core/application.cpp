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

#include "application.h"
#include "core/paths.h"

constexpr float MOVE_SPEED = 10.0f;
constexpr float MOUSE_SENSITIVITY = 0.1f;

constexpr float STICK_LOOK_SPEED = 120.0f;
constexpr double ZOOM_SPEED = 2.0;

constexpr double FIXED_DT = 1.0 / 60.0;

namespace Stellar
{
    Application::Application() :
    m_window(800, 600, "Stellar Program"),
    m_input(m_window.getHandle()),
    m_graphicsContext(),
    m_debugOverlay(m_window.getHandle()),
    m_shader(assetPath("shaders/triangle.vert"), assetPath("shaders/triangle.frag")),
    m_litShader(assetPath("shaders/lit.vert"), assetPath("shaders/lit.frag")),
    m_sphereMesh(unitSphere(32, 64)),
    m_orbitMesh(unitCircle(256), GL_LINE_LOOP),
    m_camera(glm::dvec3(10'000'000.0, 0.0f, 30.0f)),
    m_orbitCamera(10.0)
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

        if (m_input.wasPressed(Action::ToggleCamera))
        {
            m_useOrbitCamera = !m_useOrbitCamera;
        }

        m_world.vessel().burn = glm::dvec2(
            m_input.axis(Action::BurnPrograde),
            m_input.axis(Action::BurnNormal));

        if (!m_input.isCursorCaptured())
        {
            return;
        }

        const glm::dvec2 mouse = m_input.mouseDelta() * static_cast<double>(MOUSE_SENSITIVITY);

        // ORBIT CAMERA
        if (m_useOrbitCamera)
        {
            const float stickYaw = STICK_LOOK_SPEED * deltaTime * static_cast<float>(m_input.axis(Action::LookRight));
            const float stickPitch = STICK_LOOK_SPEED * deltaTime * static_cast<float>(m_input.axis(Action::LookUp));

            m_orbitCamera.rotate(static_cast<float>(mouse.x) + stickYaw, static_cast<float>(mouse.y) + stickPitch);

            // exp() keeps the factor above 0 even after a long frame, and makes zoom feel the same near and far
            m_orbitCamera.zoom(std::exp(-ZOOM_SPEED * deltaTime * m_input.axis(Action::MoveForward)));

            return;
        }

        // FREE CAMERA (debug)
        const float step = MOVE_SPEED * deltaTime;

        m_camera.moveForward(step * static_cast<float>(m_input.axis(Action::MoveForward)));
        m_camera.moveRight(step * static_cast<float>(m_input.axis(Action::MoveRight)));

        m_camera.rotate(static_cast<float>(mouse.x), static_cast<float>(mouse.y));
    }

    void Application::render(const float alpha)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // VESSEL POSITION
        const Vessel& vessel = m_world.vessel();

        const double interpolation = m_world.timeWarp() > 1.0 ? 1.0 : static_cast<double>(alpha);
        const glm::dvec3 orbitPosition = glm::mix(vessel.previousPosition, vessel.state.position, interpolation);
        const glm::dvec3 vesselPosition = CENTRAL_BODY_POSITION + orbitPosition;

        // CAMERA
        const glm::dvec3 cameraPosition = m_useOrbitCamera
            ? vesselPosition + m_orbitCamera.offset()
            : m_camera.getPosition();

        // VIEW
        const glm::mat4 viewMatrix = m_useOrbitCamera
            ? m_orbitCamera.getViewMatrix()
            : m_camera.getViewMatrix();

        // PROJECTION
        int fbWidth = 0, fbHeight = 0;
        m_window.getFramebufferSize(fbWidth, fbHeight);

        const float aspectRatio = (fbHeight > 0)
            ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight)
            : 1.0f;

        const auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

        // LIT OBJECTS (spheres)
        m_litShader.use();
        m_litShader.setMat4("uView", glm::value_ptr(viewMatrix));
        m_litShader.setMat4("uProjection", glm::value_ptr(projectionMatrix));
        m_litShader.setVec3("uLightDir", 1.0f, 0.5f, 0.3f); // Toward the "Sun", fixed for now

        // MODEL - "Earth"
        const glm::dvec3 centralRelativeToCam = CENTRAL_BODY_POSITION - cameraPosition;
        const auto centralModel = glm::translate(glm::mat4(1.0f), glm::vec3(centralRelativeToCam));

        m_litShader.setVec3("uColor", 0.25f, 0.45f, 0.9f);
        m_litShader.setMat4("uModel", glm::value_ptr(centralModel));
        m_sphereMesh.draw();

        // MODE - Satellite
        const glm::dvec3 orbiterRelativeToCam =  vesselPosition - cameraPosition;
        const auto orbiterModel = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(orbiterRelativeToCam)), glm::vec3(0.5f));

        m_litShader.setVec3("uColor", 0.9f, 0.9f, 0.9f);
        m_litShader.setMat4("uModel", glm::value_ptr(orbiterModel));
        m_sphereMesh.draw();

        // UNLIT OBJECTS (orbit line)
        m_shader.use();
        m_shader.setMat4("uView", glm::value_ptr(viewMatrix));
        m_shader.setMat4("uProjection", glm::value_ptr(projectionMatrix));
        m_shader.setVec3("uColor", 1.0f, 1.0f, 1.0f);

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
        m_debugOverlay.draw(m_world, cameraPosition);
    }
} // Stellar