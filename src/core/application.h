/**
 * @file application.h
 * @brief
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once

#include "core/window.h"
#include "graphics/graphics_context.h"
#include "graphics/shader.h"
#include "graphics/mesh.h"
#include "graphics/camera.h"
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
        Camera m_camera;

        double  m_lastFrameTime = 0.0;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;

        double m_accumulatedTime = 0.0;

        glm::dvec3 m_cubePosition = glm::dvec3(0.0);
        glm::vec3 m_cubeRotationAxis = glm::vec3(1.0f, 1.0f, 0.0f);

        float m_cubeAngle = 0.0f;
        float m_previousCubeAngle = 0.0f;

        bool m_cursorCaptured = true;
        bool m_tabWasPressed = false;

        void processInput(float deltaTime);
        void render(float alpha) const;
        void update(double deltaTime);
    };
} // Stellar
