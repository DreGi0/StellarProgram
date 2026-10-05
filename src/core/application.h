/**
 * @file application.h
 * @brief Top-level orchestrator: owns every subsystem and runs the main loop.
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
#include "graphics/orbit_camera.h"
#include "graphics/framebuffer.h"
#include "ui/debug_overlay.h"
#include "audio/audio.h"

namespace Stellar
{
    /**
     * @class Application
     * @brief Creates the window and every subsystem, then runs the loop:
     *        input -> fixed-step simulation -> render.
     */
    class Application {
    public:
        /**
         * @brief Creates every subsystem in order (see the members) and sets the initial GL state.
         * @throws std::runtime_error If the window, GLAD or a shader fails to initialize.
         */
        Application();

        /**
         * @brief Runs the main loop until the window is closed.
         *
         * The simulation advances in fixed steps of 1/60 s using an accumulator;
         * rendering interpolates between the last two steps.
         */
        void run();

    private:
        // ORDER MATTERS: constructed top to bottom and destroyed bottom to top
        Window m_window;
        Input m_input;
        Audio m_audio;
        GraphicsContext m_graphicsContext;
        Framebuffer m_framebuffer;
        DebugOverlay m_debugOverlay;
        Shader m_shader;
        Shader m_litShader;
        Mesh m_sphereMesh;
        Mesh m_orbitMesh;
        Camera m_camera;
        OrbitCamera m_orbitCamera;

        double m_lastFrameTime = 0.0;   ///< glfwGetTime() at the start of the previous frame
        double m_accumulatedTime = 0.0; ///< Real time not yet consumed by fixed steps
        bool m_useOrbitCamera = true; ///< true = orbit camera around the vessel, false = free-look debug camera

        World m_world;

        /**
         * @brief Reads the input once per frame: cursor toggle, camera switch, burn command and the active camera.
         * @param deltaTime Seconds since the last frame (for camera movement).
         */
        void processInput(float deltaTime);

        /**
         * @brief Draws the scene and the debug overlay.
         * @param alpha How far between the last two simulation steps we are, in [0, 1).
         */
        void render(float alpha);
    };
} // Stellar
