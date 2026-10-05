/**
 * @file framebuffer.h
 * @brief Off-screen render target with a 32-bit float depth buffer.
 * @author DreGi0
 * @date October 5th, 2026
 */
#pragma once

#include <glad/glad.h>

namespace Stellar
{
    /**
     * @class Framebuffer
     * @brief The scene is drawn here instead of the window, then copied to the window.
     *
     * Exists to choose the depth format (float 32) that the default window framebuffer does not offer.
     */
    class Framebuffer
    {
        public:
        /**
         * @brief Creates the framebuffer with a color and a depth attachment.
         * @throws std::runtime_error If the framebuffer is not complete.
         */
        Framebuffer(int width, int height);

        // Disable copy to avoid duplicity of OpenGL handles
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        ~Framebuffer();

        /**
         * @brief Reallocates the attachments when the size changes. Ignores 0 (minimized window).
         */
        void resize(int width, int height);

        /**
         * @brief Draw calls go here from now on.
         */
        void bind() const;

        /**
         * @brief Copies the color to the window and binds the window again (for ImGui).
         */
        void blitToScreen() const;

        private:
        GLuint m_fbo = 0;
        GLuint m_color = 0; ///< Renderbuffer: nothing reads it yet; becomes a texture for post-processing
        GLuint m_depth = 0;
        int m_width = 0;
        int m_height = 0;
    };
} // Stellar


