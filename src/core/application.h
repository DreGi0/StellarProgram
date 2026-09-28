/**
 * @file application.h
 * @brief
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once

#include <glm/vec2.hpp>

#include "core/window.h"
#include "graphics/graphics_context.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/camera.h"
#include "physics/orbit.h"
#include "ui/debug_overlay.h"

namespace Stellar
{
    class Application {
    public:
        Application();
        void run();

    private:
        // ORDER MATTERS: constructed top to bottom and destroyed bottom to top
        Window m_window;
        GraphicsContext m_graphicsContext;
        DebugOverlay m_debugOverlay;
        Shader m_shader;
        Mesh m_cubeMesh;
        Mesh m_orbitMesh;
        Camera m_camera;

        double  m_lastFrameTime = 0.0;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;

        double m_accumulatedTime = 0.0;

        OrbitalState m_orbitState{};
        glm::dvec3 m_previousOrbitPosition = glm::dvec3(0.0);
        double m_initialEnergy = 0.0;
        // 0: Explicit Euler, 1:SemiImplicitEuler 2: Velocity Verlet, 3: Kepler
        int m_integratorIndex = 3;
        int m_timeWarpIndex = 0;
        double m_missionTime = 0.0;
        glm::dvec2 m_burnInput = glm::dvec2(0.0);

        bool m_cursorCaptured = true;
        bool m_tabWasPressed = false;

        void processInput(float deltaTime);
        void render(float alpha);
        void update(double deltaTime);
        void resetOrbit();
        double timeWarp() const;
        bool isBurning() const;
        double physicsWarp() const;
    };
} // Stellar
