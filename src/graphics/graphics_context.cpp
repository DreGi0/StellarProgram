/**
 * @file graphics_context.cpp
 * @brief
 * @author DreGi0
 * @date September 25th, 2026
 */

#include "graphics_context.h"

#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>

#include "gl_debug.h"

namespace Stellar
{
    GraphicsContext::GraphicsContext()
    {
        if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        printf("[GLAD] Loaded OpenGL Version Macro: %d.%d\n", GLVersion.major, GLVersion.minor);

        const auto glVersion   = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        const auto glRenderer  = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const auto glVendor    = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
        const auto glslVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        printf("[GLAD] OpenGL Version: %s\n", glVersion ? glVersion : "Unknown");
        printf("[GLAD] GPU: %s\n", glRenderer ? glRenderer : "Unknown");
        printf("[GLAD] Vendor: %s\n", glVendor ? glVendor : "Unknown");
        printf("[GLAD] GLSL Version: %s\n", glslVersion ? glslVersion : "Unknown");
        fflush(stdout);

#ifdef STELLAR_DEBUG
        Stellar::enableDebugOutput();
#endif
    }
} // Stellar