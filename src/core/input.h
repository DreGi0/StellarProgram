/**
 * @file input.h
 * @brief Keyboard and mouse state read from GLFW.
 * @author DreGi0
 * @date September 27th, 2026
 */

#pragma once

#include <array>
#include <cstddef>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace Stellar
{
    /// Things the player can do, independent of the device (keyboard or gamepad).
    enum class Action { ToggleCursor, BurnPrograde, BurnNormal, MoveForward, MoveRight, ToggleCamera, LookRight, LookUp, Count };

    /**
     * @class Input
     * @brief Reads the keyboard and the mouse of a GLFW window and owns the cursor capture.
     *
     * It does not own the window: it only keeps the pointer, so the window must outlive it.
     */
    class Input
    {
        public:
        /**
         * @brief Captures the cursor and stores the initial mouse position.
         * @param window Window to read the input from.
         */
        explicit Input(GLFWwindow* window);

        /**
         * @brief Reads the keyboard and the gamepad. Call it once per frame, before asking for actions.
         */
        void update();

        /**
         * @brief Value of an action, from the keyboard or the gamepad (whichever is pushed more).
         * @param action Action to read.
         * @return -1..1; keys give exactly -1, 0 or 1, sticks give anything in between.
         */
        [[nodiscard]] double axis(Action action) const;

        /**
         * @brief Checks if an action became active this frame.
         * @param action Action to check.
         * @return true only on the frame it goes from 0 to non-zero.
         */
        [[nodiscard]] bool wasPressed(Action action) const;

        /**
         * @brief Checks if the cursor is captured (hidden and locked to the window).
         * @return true while captured.
         */
        [[nodiscard]] bool isCursorCaptured() const;

        /**
         * @brief How much the mouse moved since the last call. Y is positive when the mouse moves up.
         * @return Movement in pixels.
         */
        glm::dvec2 mouseDelta();

        /**
         * @brief Captures or releases the cursor and re-reads its position so the camera does not jump.
         * @param captured true to capture, false to release.
         */
        void setCursorCaptured(bool captured);

        private:
        [[nodiscard]] bool isDown(int key) const;
        GLFWwindow* m_window;

        std::array<double, static_cast<std::size_t>(Action::Count)> m_current {}, m_previous {};

        bool m_cursorCaptured = true;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;
    };
} // Stellar
