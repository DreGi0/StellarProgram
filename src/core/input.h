/**
 * @file input.h
 * @brief Keyboard and mouse state read from GLFW.
 * @author DreGi0
 * @date September 27th, 2026
 */

#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace Stellar
{
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
         * @brief Checks if a key is held down right now.
         * @param key GLFW key code (e.g. GLFW_KEY_W).
         * @return true while the key is pressed.
         */
        [[nodiscard]] bool isDown(int key) const;

        /**
         * @brief Turns two opposite keys into one value, like a joystick axis.
         * @param positiveKey Key that pushes the value to +1.
         * @param negativeKey Key that pushes the value to -1.
         * @return +1, -1, or 0 when neither or both are pressed.
         */
        [[nodiscard]] double axis(int positiveKey, int negativeKey) const;

        /**
         * @brief Checks if a key went down this frame (it was up the last time it was asked).
         * @warning Call it once per frame per key; a second call in the same frame returns false.
         * @param key GLFW key code.
         * @return true only on the frame the key is pressed.
         */
        [[nodiscard]] bool wasPressed(int key);

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
        GLFWwindow* m_window;

        std::array<bool, GLFW_KEY_LAST + 1> m_previous {}; // Key state seen by the last wasPressed call

        bool m_cursorCaptured = true;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;
    };
} // Stellar
