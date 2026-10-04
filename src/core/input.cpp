/**
 * @file input.cpp
 * @brief Implementation of Input over GLFW.
 * @author DreGi0
 * @date September 27th, 2026
 */

#include "input.h"

#include <cmath>

namespace
{
    using Stellar::Action;

    /// Where one action comes from on each device.
    struct Binding
    {
        int positiveKey;    ///< Key that pushes the value to +1
        int negativeKey;    ///< Key that pushes it to -1 (GLFW_KEY_UNKNOWN if none)
        int gamepadAxis;    ///< GLFW_GAMEPAD_AXIS_* (-1 if none)
        double gamepadSign; ///< Flips the stick: GLFW's Y is +1 when pushed down
    };

    // Same order as the Action enum
    constexpr std::array<Binding, static_cast<std::size_t>(Action::Count)> BINDINGS {{
        {
            .positiveKey = GLFW_KEY_TAB,
            .negativeKey = GLFW_KEY_UNKNOWN,
            .gamepadAxis = -1,
            .gamepadSign = 0.0
        }, // ToggleCursor
        {
            .positiveKey = GLFW_KEY_UP,
            .negativeKey = GLFW_KEY_DOWN,
            .gamepadAxis = GLFW_GAMEPAD_AXIS_LEFT_Y,
            .gamepadSign = -1.0
        }, // BurnPrograde
        {
            .positiveKey = GLFW_KEY_RIGHT,
            .negativeKey = GLFW_KEY_LEFT,
            .gamepadAxis = GLFW_GAMEPAD_AXIS_LEFT_X,
            .gamepadSign = 1.0
        }, // BurnNormal
        {
            .positiveKey = GLFW_KEY_W,
            .negativeKey = GLFW_KEY_S,
            .gamepadAxis = -1,
            .gamepadSign = 0.0
        }, // MoveForward
        {
            .positiveKey = GLFW_KEY_D,
            .negativeKey = GLFW_KEY_A,
            .gamepadAxis = -1,
            .gamepadSign = 0.0
        }, // MoveRight
        {
            .positiveKey = GLFW_KEY_C,
            .negativeKey = GLFW_KEY_UNKNOWN,
            .gamepadAxis = -1,
            .gamepadSign = 0.0
        }, // ToggleCamera
        {
            .positiveKey = GLFW_KEY_UNKNOWN,
            .negativeKey = GLFW_KEY_UNKNOWN,
            .gamepadAxis = GLFW_GAMEPAD_AXIS_RIGHT_X,
            .gamepadSign = 1.0
        }, // LookRight
        {
            .positiveKey = GLFW_KEY_UNKNOWN,
            .negativeKey = GLFW_KEY_UNKNOWN,
            .gamepadAxis = GLFW_GAMEPAD_AXIS_RIGHT_Y,
            .gamepadSign = -1.0
        }, // LookUp
    }};

    constexpr double STICK_DEADZONE = 0.15;
}

namespace Stellar
{
    Input::Input(GLFWwindow* window) :
    m_window(window)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
    }

    void Input::update()
    {
        m_previous = m_current;

        // Returns false when no gamepad is connected, so plugging/unplugging just works
        GLFWgamepadstate pad {};
        const bool hasPad = glfwGetGamepadState(GLFW_JOYSTICK_1, &pad) == GLFW_TRUE;

        for (std::size_t i = 0; i < BINDINGS.size(); ++i)
        {
            const Binding& binding = BINDINGS[i];

            const double keyboard = isDown(binding.positiveKey) - isDown(binding.negativeKey);

            double stick = 0.0;
            if (hasPad && binding.gamepadAxis != -1)
            {
                stick = binding.gamepadSign * pad.axes[binding.gamepadAxis];

                if (std::abs(stick) < STICK_DEADZONE)
                {
                    stick = 0.0;
                }
            }

            m_current[i] = std::abs(stick) > std::abs(keyboard) ? stick : keyboard;
        }
    }

    double Input::axis(Action action) const
    {
        return m_current[static_cast<std::size_t>(action)];
    }

    bool Input::wasPressed(Action action) const
    {
        const auto i = static_cast<std::size_t>(action);

        return m_current[i] != 0.0 && m_previous[i] == 0.0;
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

    bool Input::isDown(const int key) const
    {
        return key != GLFW_KEY_UNKNOWN && glfwGetKey(m_window, key) == GLFW_PRESS;
    }

    bool Input::isCursorCaptured() const
    {
        return m_cursorCaptured;
    }
} // Stellar
