/**
 * @file window.h
 * @brief Handle window lifecycle & OpenGL context through RAII.
 * @author DreGi0
 * @date September 12th, 2026
 */

#pragma once

#include <memory>

#include "GLFW/glfw3.h"

namespace Stellar {
    /**
     * @struct WindowDeleter
     * @brief Custom deleter functor for std::unique_ptr.
     */
    struct WindowDeleter {
        /**
         * @brief Destroys the window and terminates GLFW.
         * @param window Window handle to destroy.
         */
        void operator()(GLFWwindow* window) const noexcept {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    };
    /**
     * @class Window
     * @brief Encapsulates a GLFW window and an OpenGL 4.6 Core rendering context.
     */
    class Window {

        public:
        /**
         * @brief Creates a GLFW window and initializes the OpenGL context.
         * @param width Initial width of the framebuffer in pixels.
         * @param height Initial height of the framebuffer in pixels.
         * @param title Visible title in the window bar.
         * @throws std::runtime_error If GLFW initialization or window creation fails.
         */
        Window(int width, int height, const char* title);

        // Disable copy to preserve unique ownership of the resource
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /// Move constructor. Transfers the window handle; the moved-from object is left empty.
        Window(Window&&) = default;

        /**
         * @brief Move assignment. Transfers the window handle; the moved-from object is left empty.
         * @return Reference to this instance.
         */
        Window& operator=(Window&&) = default;

        /**
         * @brief Checks if the user requested to close the window.
         * @return true if it should, false otherwise.
         */
        [[nodiscard]] bool shouldClose() const noexcept;

        /**
         * @brief Swap front & back buffer (Double buffering).
         */
        void swapBuffers() const;

        /**
         * @brief Process queued events in GLFW (keyboard, mouse, window).
         */
        static void pollEvents();

        /**
         * @brief Get current framebuffer dimensions in pixels
         * @param[out] width References where the width is stored
         * @param[out] height References where the height is stored
         */
        void getFramebufferSize(int& width, int& height) const;

        /**
         * @brief Gets the underlying GLFW pointer.
         * @return Raw pointer to the managed GLFW window structure.
         */
        [[nodiscard]] GLFWwindow* getHandle() const { return m_handle.get(); }

        private:
        // Smart pointer
        std::unique_ptr<GLFWwindow, WindowDeleter> m_handle;

        // GLFW event callbacks
        static void errorCallback(int error, const char* description);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    };
} // Stellar