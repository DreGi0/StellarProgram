/**
 * @file debug_overlay.h
 * @brief
 * @author DreGi0
 * @date September 26th, 2026
 */

#pragma once

struct GLFWwindow;

namespace Stellar
{
    class DebugOverlay
    {
        public:
        explicit DebugOverlay(GLFWwindow* window);

        DebugOverlay(const DebugOverlay&) = delete;
        DebugOverlay& operator=(const DebugOverlay&) = delete;

        DebugOverlay(DebugOverlay&&) = delete;

        DebugOverlay& operator=(DebugOverlay&&) = delete;

        ~DebugOverlay();

        void beginFrame() const;

        void endFrame() const;
    };
} // Stellar