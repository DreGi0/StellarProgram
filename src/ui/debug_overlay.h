/**
 * @file debug_overlay.h
 * @brief Dear ImGui lifetime (RAII) and the debug panel.
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

struct GLFWwindow;

namespace Stellar
{
    class World;
    class Camera;

    class DebugOverlay
    {
        public:
        explicit DebugOverlay(GLFWwindow* window);

        DebugOverlay(const DebugOverlay&) = delete;
        DebugOverlay& operator=(const DebugOverlay&) = delete;

        DebugOverlay(DebugOverlay&&) = delete;

        DebugOverlay& operator=(DebugOverlay&&) = delete;

        ~DebugOverlay();

        /**
         * @brief Draws the debug panel. Reads the world and the camera, and lets the user
         *        change the integrator, the time warp and reset the orbit.
         */
        void draw(World& world, const Camera& camera) const;

        private:
        void beginFrame() const;

        void endFrame() const;
    };
} // Stellar