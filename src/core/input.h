/**
 * @file input.h
 * @brief
 * @author DreGi0
 * @date September 27th, 2026
 */

#pragma once

#include <array>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>

namespace Stellar
{
    class Input
    {
        public:
        explicit Input(GLFWwindow* window);

        [[nodiscard]] bool isDown(int key) const;

        [[nodiscard]] double axis(int positiveKey, int negativeKey) const;

        [[nodiscard]] bool wasPressed(int key);

        [[nodiscard]] bool isCursorCaptured() const;

        glm::dvec2 mouseDelta();

        void setCursorCaptured(bool captured);

        private:
        GLFWwindow* m_window;

        std::array<bool, GLFW_KEY_LAST + 1> m_previous {};

        bool m_cursorCaptured = true;
        double m_lastMouseX = 0.0;
        double m_lastMouseY = 0.0;
    };
} // Stellar
