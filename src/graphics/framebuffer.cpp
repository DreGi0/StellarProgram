/**
 * @file framebuffer.cpp
 * @brief Framebuffer implementation
 * @author DreGi0
 * @date October 5th, 2026
 */

#include "framebuffer.h"

#include <stdexcept>

namespace Stellar
{
    Framebuffer::Framebuffer(const int width, const int height)
    {
        glCreateFramebuffers(1, &m_fbo);
        glCreateRenderbuffers(1, &m_color);
        glCreateRenderbuffers(1, &m_depth);

        resize(width, height);

        glNamedFramebufferRenderbuffer(m_fbo, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_color);
        glNamedFramebufferRenderbuffer(m_fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth);

        if (glCheckNamedFramebufferStatus(m_fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("Framebuffer is not complete");
        }
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteRenderbuffers(1, &m_depth);
        glDeleteRenderbuffers(1, &m_color);
        glDeleteFramebuffers(1, &m_fbo);
    }

    void Framebuffer::resize(const int width, const int height)
    {
        if (width <= 0 || height <= 0 || (width == m_width && height == m_height))
        {
            return;
        }

        m_width = width;
        m_height = height;

        // Calling storage again on the same renderbuffer just replaces its memory
        glNamedRenderbufferStorage(m_color, GL_RGBA8, width, height);
        glNamedRenderbufferStorage(m_depth, GL_DEPTH_COMPONENT32F, width, height);
    }

    void Framebuffer::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void Framebuffer::blitToScreen() const
    {
        // 0 = the window's framebuffer
        glBlitNamedFramebuffer(m_fbo, 0,
            0, 0, m_width, m_height,
            0, 0, m_width, m_height,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // Stellar