/**
 * @file window.cpp
 * @brief Window class implementation and GLFW callback configuration
 * @author DreGi0
 * @date September 12th, 2026
 */
#include <glad/glad.h>

#include "window.h"

namespace Stellar {

    // ----- CONTRUCTORS & METHODS ----------
    Window::Window(const int width, const int height, const char *title) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW3");
        }

        // OpenGL 4.6 core
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Remove depreciated features

#ifdef STELLAR_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        m_handle.reset(window);

        glfwMakeContextCurrent(m_handle.get());
        glfwSwapInterval(1); // Enable V-Sync by default

        // Bind event callbacks to active window
        glfwSetKeyCallback(m_handle.get(), key_callback);
        glfwSetFramebufferSizeCallback(m_handle.get(), framebuffer_size_callback);
    }

    bool Window::shouldClose() const noexcept {
        return glfwWindowShouldClose(m_handle.get());
    }

    void Window::swap_buffers() const {
        glfwSwapBuffers(m_handle.get());
    }

    void Window::poll_events() {
        glfwPollEvents();
    }

    void Window::get_framebuffer_size(int &width, int &height) const {
        glfwGetFramebufferSize(m_handle.get(), &width, &height);
    }

    // ----- CALLBACKS ----------
    void Window::error_callback(int /*error*/, const char *description) {
        fprintf(stderr, "Error: %s\n", description);
    }

    void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        // Close window on ESC key pressed
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }

    void Window::framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
        glViewport(0, 0, width, height);
    }
} // Stellar