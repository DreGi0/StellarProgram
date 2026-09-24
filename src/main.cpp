
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/paths.h"
#include "core/window.h"
#include "graphics/mesh.h"
#include "graphics/shader.h"

/**
 * @brief Entry point. Creates the window, loads GL, and runs the render loop.
 * @return EXIT_SUCCESS on clean shutdown, EXIT_FAILURE if initialization throws.
 */
int main() {
    try {
        // Initialize GLFW window
        const Stellar::Window window(800, 600, "Stellar Program");

        if (const int version = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)); version == 0) {
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

        // Viewport adjustment
        int fbWidth = 0;
        int fbHeight = 0;
        window.get_framebuffer_size(fbWidth, fbHeight);
        glViewport(0, 0, fbWidth, fbHeight);

        // Load and compile shaders
        const Stellar::Shader shaderProgram {
            Stellar::asset_path("shaders/triangle.vert"),
            Stellar::asset_path("shaders/triangle.frag")
        };
        printf("[Shader] Program created OK (id=%u)\n", shaderProgram.get_id());

        // Model geometry
        constexpr float vertices[] = {
            // Position (x, y, z)  |  Color (r, g, b)
            0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,   // triangle 1: up-right - blue
            -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   // triangle 1: low-left - green
            0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f,   // triangle 1: low-right - red

            0.5f, 0.5f, 0.0f,       0.0f, 0.0f, 1.0f,   // triangle 2: up-right - blue
            -0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,   // triangle 2: up-left - red
            -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   // triangle 2: low-left - green
        };

        // Mesh
        const Stellar::Mesh triangleMesh(vertices, 6);

        // Background color used by glClear() on every frame
        glClearColor(0.0f, 0.07f, 0.12f, 1.0f);

        // Main rendering loop
        while (!window.shouldClose()) {
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram.use();
            shaderProgram.set_vec3("uColor", 1.0f, 1.0f, 1.0f);

            auto modelMatrix = glm::mat4(1.0f);
            shaderProgram.set_mat4("uModel", glm::value_ptr(modelMatrix));

            triangleMesh.draw();

            window.swap_buffers();
            Stellar::Window::poll_events();
        }
        fflush(stdout);
    } catch (const std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
