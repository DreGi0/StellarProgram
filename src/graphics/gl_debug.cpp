/**
 * @file gl_debug.cpp
 * @brief OpenGL 4.3+ debug output setup.
 * @author DreGi0
 * @date September 12th, 2026
 */

#include "gl_debug.h"

#include <cstdio>
#include <glad/glad.h>


namespace Stellar {
    /// Maps a GL_DEBUG_SEVERITY_* enum to a short printable label.
    static const char* severityToString(const GLenum severity)
    {
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         return "HIGH";
        case GL_DEBUG_SEVERITY_MEDIUM:       return "MEDIUM";
        case GL_DEBUG_SEVERITY_LOW:          return "LOW";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "info";
        default:                             return "?";
        }
    }

    /// Callback invoked by the driver for each debug message. Signature fixed by glDebugMessageCallback.
    static void APIENTRY debugCallback(GLenum /*source*/, GLenum /*type*/, GLuint /*id*/, const GLenum severity,
                                        GLsizei /*length*/, const GLchar* message, const void* /*userParam*/) {

        fprintf(stderr, "[GL %s] %s\n", severityToString(severity), message);
    }

    void enableDebugOutput() {
        if (!GLAD_GL_VERSION_4_3) {
            fprintf(stderr, "[gl_debug] KHR_debug not available\n");
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // ! -> ONLY FOR DEVELOPMENT
        glDebugMessageCallback(debugCallback, nullptr);

        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
                              0, nullptr, GL_FALSE);

        fprintf(stderr, "[gl_debug] debug output enabled\n");
    }
} // Stellar
