/**
 * @file application.h
 * @brief
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once

#include <glm/vec2.hpp>

#include "core/window.h"
#include "core/input.h"
#include "game/world.h"
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
        Input m_input;
        GraphicsContext m_graphicsContext;
        DebugOverlay m_debugOverlay;
        Shader m_shader;
        Mesh m_cubeMesh;
        Mesh m_orbitMesh;
        Camera m_camera;

        double  m_lastFrameTime = 0.0;

        double m_accumulatedTime = 0.0;

        World m_world;

        void processInput(float deltaTime);
        void render(float alpha);
    };
} // Stellar
