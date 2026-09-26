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
        Shader m_shader;
        Mesh m_cubeMesh;
        Camera m_camera;

        double  m_lastFrameTime = 0.0;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;

        double m_accumulatedTime = 0.0;

        float m_cubeAngle = 0.0f;
        float m_previousCubeAngle = 0.0f;

        void processInput(float deltaTime);
        void render(float alpha) const;
        void update(double deltaTime);
    };
} // Stellar
