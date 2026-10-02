/**
 * @file input.cpp
 * @brief
 * @author DreGi0
 * @date September 27th, 2026
 */

#include "input.h"

namespace Stellar
{
    Input::Input(GLFWwindow* window) :
    m_window(window)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
    }

    bool Input::isDown(const int key) const
    {
        return glfwGetKey(m_window, key) == GLFW_PRESS;
    }

    double Input::axis(const int positiveKey, const int negativeKey) const
    {
        return isDown(positiveKey) - isDown(negativeKey);
    }

    bool Input::wasPressed(const int key)
    {
        const bool keyIsDown = isDown(key);

        const bool justPressed = keyIsDown && !m_previous[key];

        m_previous[key] = keyIsDown;

        return justPressed;

    }

    glm::dvec2 Input::mouseDelta()
    {
        double mouseX = 0, mouseY = 0;
        glfwGetCursorPos(m_window, &mouseX, &mouseY);

        const double offsetX = mouseX - m_lastMouseX;
        const double offsetY = m_lastMouseY - mouseY;

        m_lastMouseX = mouseX;
        m_lastMouseY = mouseY;

        return glm::dvec2(offsetX, offsetY);
    }

    void Input::setCursorCaptured(const bool captured)
    {
        m_cursorCaptured = captured;

        glfwSetInputMode(m_window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
    }

    bool Input::isCursorCaptured() const
    {
        return m_cursorCaptured;
    }
} // Stellar
