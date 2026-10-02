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
#include <imgui.h>
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

constexpr double THRUST_ACCELERATION = 1.0;
constexpr double MAX_PHYSICS_WARP = 4.0;

// Orbit (toy units: r = 10, v = 10 -> which is one lap every 2*pi seconds)
constexpr double ORBIT_MU = 1000.0;
constexpr glm::dvec3 CENTRAL_BODY_POSITION(10'000'000.0, 0.0, 0.0);

// Starting orbit, described with elements instead of position + velocity
constexpr Stellar::OrbitalElements START_ORBIT {
    .semiMajorAxis = 10.0,
    .eccentricity = 0.3,
    .inclination = glm::radians(20.0),
    .longitudeOfAscendingNode = 0.0,
    .argumentOfPeriapsis = 0.0,
    .trueAnomaly = 0.0,
};

// note: it only needs one (Velocity Verlet), but since I'm trying to see what happens on each
constexpr Stellar::StepFunction INTEGRATORS[] = {
    Stellar::stepExplicitEuler,
    Stellar::stepSemiImplicitEuler,
    Stellar::stepVelocityVerlet,
    Stellar::stepKepler,
};

constexpr const char* INTEGRATOR_NAMES[] = {
    "Explicit Euler",
    "Semi-implicit Euler",
    "Velocity Verlet",
    "Kepler (on rails)",
};

constexpr double TIME_WARP_LEVELS[] = {
    1.0, 5.0, 10.0, 50.0, 100.0, 1'000.0, 10'000.0, 100'000.0
};

constexpr const char* TIME_WARP_NAMES[] = {
    "x1", "x5", "x10", "x50", "x100", "x1000", "x10000", "x100000",
};


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
        resetOrbit();

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

            processInput(static_cast<float>(frameTime));

            frameTime = glm::min(frameTime, 0.25);
            m_accumulatedTime += frameTime * physicsWarp();

            while (m_accumulatedTime >= FIXED_DT)
            {
                m_previousOrbitPosition = m_orbitState.position;

                update(FIXED_DT);
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
        if (m_input.wasPressed(GLFW_KEY_TAB))
        {
            m_input.setCursorCaptured(!m_input.isCursorCaptured());
        }

        m_burnInput = glm::dvec2(
            m_input.axis(GLFW_KEY_UP, GLFW_KEY_DOWN),
            m_input.axis(GLFW_KEY_RIGHT, GLFW_KEY_LEFT));

        if (!m_input.isCursorCaptured())
        {
            return;
        }

        // CAMERA
        const float step = MOVE_SPEED * deltaTime;

        m_camera.moveForward(step * static_cast<float>(m_input.axis(GLFW_KEY_W, GLFW_KEY_S)));
        m_camera.moveRight(step * static_cast<float>(m_input.axis(GLFW_KEY_D, GLFW_KEY_A)));

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
        const double interpolation = timeWarp() > 1.0 ? 1.0 : static_cast<double>(alpha);
        const glm::dvec3 orbitPosition = glm::mix(m_previousOrbitPosition, m_orbitState.position, interpolation);
        const glm::dvec3 orbiterRelativeToCam = CENTRAL_BODY_POSITION + orbitPosition - m_camera.getPosition();
        const auto orbiterModel = glm::translate(glm::mat4(1.0f), glm::vec3(orbiterRelativeToCam));

        m_shader.setMat4("uModel", glm::value_ptr(orbiterModel));
        m_cubeMesh.draw();

        // MODEL - Orbit line (ellipses only)
        const OrbitalElements elements = stateToElements(m_orbitState, ORBIT_MU);

        if (elements.eccentricity < 1.0)
        {
            const glm::dmat4 orbitModel = glm::translate(glm::dmat4(1.0), centralRelativeToCam) * orbitEllipseMatrix(elements);
            const glm::mat4 orbitModelF(orbitModel);

            m_shader.setMat4("uModel", glm::value_ptr(orbitModelF));
            m_orbitMesh.draw();
        }

        // DEBUG OVERLAY
        const double energy = specificEnergy(m_orbitState, ORBIT_MU);
        const double drift = glm::abs(energy - m_initialEnergy) / glm::abs(m_initialEnergy);

        m_debugOverlay.beginFrame();

        ImGui::Begin("Debug");
        ImGui::Text("Stellar Program");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::Text("Camera Position:\nX %.3f, Y %.3f, Z %.3f", m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);

        ImGui::Separator();

        if (ImGui::Combo("Integrator", &m_integratorIndex, INTEGRATOR_NAMES, IM_ARRAYSIZE(INTEGRATOR_NAMES)))
        {
            resetOrbit();
        }

        if (ImGui::Button("Reset orbit"))
        {
            resetOrbit();
        }

        // Big steps would wreck the step-by-step integrators; only Kepler is exact for any step
        const bool onRails = INTEGRATORS[m_integratorIndex] == stepKepler;

        ImGui::BeginDisabled(!onRails);
        ImGui::Combo("Time warp", &m_timeWarpIndex, TIME_WARP_NAMES, IM_ARRAYSIZE(TIME_WARP_NAMES));
        ImGui::EndDisabled();

        if (!onRails)
        {
            ImGui::TextDisabled("Time warp needs Kepler (on rails)");
        }

        ImGui::Text("Mission time: %.1f s", m_missionTime);

        if (isBurning())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "ENGINE ON - off rails (physics warp x%.0f)", physicsWarp());        }
        else
        {
            ImGui::Text("Engine off - %s", onRails ? "on rails" : "step by step");
        }

        ImGui::TextDisabled("Burn: Up/Down prograde/retrograde, Right/Left normal/antinormal");

        ImGui::Text("Orbit radius: %.4f", glm::length(m_orbitState.position));
        ImGui::Text("Energy: %.6f", energy);
        ImGui::Text("Energy drift: %.12f", drift);

        ImGui::Separator();

        ImGui::Text("Periapsis: %.4f", elements.semiMajorAxis * (1.0 - elements.eccentricity));
        ImGui::Text("Apoapsis:  %.4f", elements.semiMajorAxis * (1.0 + elements.eccentricity));
        ImGui::Text("a (semi-major axis): %.6f", elements.semiMajorAxis);
        ImGui::Text("e (eccentricity): %.6f", elements.eccentricity);
        ImGui::Text("i (inclination): %.3f deg", glm::degrees(elements.inclination));
        ImGui::Text("LAN (Omega): %.3f deg", glm::degrees(elements.longitudeOfAscendingNode));
        ImGui::Text("Arg. periapsis (w): %.3f deg", glm::degrees(elements.argumentOfPeriapsis));
        ImGui::Text("True anomaly (nu): %.3f deg", glm::degrees(elements.trueAnomaly));

        ImGui::End();

        m_debugOverlay.endFrame();
    }

    void Application::update(const double deltaTime)
    {
        if (isBurning())
        {
            // Engine on: the orbit is changing, so leave the rails and step the physics with Verlet
            const glm::dvec3 thrust = THRUST_ACCELERATION * burnDirection(m_orbitState, m_burnInput.x, m_burnInput.y);

            m_orbitState = stepVelocityVerletWithThrust(m_orbitState, ORBIT_MU, deltaTime, thrust);
            m_missionTime += deltaTime;

            // The burn changes the energy on purpose; measure drift from the new orbit instead
            m_initialEnergy = specificEnergy(m_orbitState, ORBIT_MU);
            return;
        }

        const double warpedDt = deltaTime * timeWarp();

        m_orbitState = INTEGRATORS[m_integratorIndex](m_orbitState, ORBIT_MU, warpedDt);
        m_missionTime += warpedDt;
    }

    void Application::resetOrbit()
    {
        m_orbitState = elementsToState(START_ORBIT, ORBIT_MU);
        m_previousOrbitPosition = m_orbitState.position;
        m_initialEnergy = specificEnergy(m_orbitState, ORBIT_MU);

        m_missionTime = 0.0;
    }

    double Application::timeWarp() const
    {
        const bool onRails = INTEGRATORS[m_integratorIndex] == stepKepler;

        return onRails && !isBurning() ? TIME_WARP_LEVELS[m_timeWarpIndex] : 1.0;
    }

    bool Application::isBurning() const
    {
        return m_burnInput != glm::dvec2(0.0);
    }

    double Application::physicsWarp() const
    {
        return isBurning() ? glm::min(TIME_WARP_LEVELS[m_timeWarpIndex], MAX_PHYSICS_WARP) : 1.0;
    }
} // Stellar