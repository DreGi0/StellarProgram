
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/paths.h"
#include "core/window.h"
#include "graphics/camera.h"
#include "graphics/gl_debug.h"
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

#ifdef STELLAR_DEBUG
        Stellar::enableDebugOutput();
#endif

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
            // Position (x, y, z)     |  Color (r, g, b)
            // Front (+Z) - red
            -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // A
             0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // B
             0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // C
            -0.5f,  0.5f,  0.5f,       1.0f, 0.0f, 0.0f,   // D

            // Back (-Z) - green
             0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // F
            -0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // E
            -0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // H
             0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 0.0f,   // G

            // Right (+X) - blue
             0.5f, -0.5f,  0.5f,       0.0f, 0.0f, 1.0f,   // B
             0.5f, -0.5f, -0.5f,       0.0f, 0.0f, 1.0f,   // F
             0.5f,  0.5f, -0.5f,       0.0f, 0.0f, 1.0f,   // G
             0.5f,  0.5f,  0.5f,       0.0f, 0.0f, 1.0f,   // C

            // Left (-X) - yellow
            -0.5f, -0.5f, -0.5f,       1.0f, 1.0f, 0.0f,   // E
            -0.5f, -0.5f,  0.5f,       1.0f, 1.0f, 0.0f,   // A
            -0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 0.0f,   // D
            -0.5f,  0.5f, -0.5f,       1.0f, 1.0f, 0.0f,   // H

            // Top (+Y) - cyan
            -0.5f,  0.5f,  0.5f,       0.0f, 1.0f, 1.0f,   // D
             0.5f,  0.5f,  0.5f,       0.0f, 1.0f, 1.0f,   // C
             0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,   // G
            -0.5f,  0.5f, -0.5f,       0.0f, 1.0f, 1.0f,   // H

            // Bottom (-Y) - magenta
            -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,   // E
             0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,   // F
             0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,   // B
            -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 1.0f,   // A
        };

        // Index data: 2 triangles per face, 4 vertices per face
        constexpr unsigned int indices[] = {
            0,  1,  2,    2,  3,  0,   // Front
            4,  5,  6,    6,  7,  4,   // Back
            8,  9, 10,   10, 11,  8,   // Right
           12, 13, 14,   14, 15, 12,   // Left
           16, 17, 18,   18, 19, 16,   // Top
           20, 21, 22,   22, 23, 20,   // Bottom
       };

        // Mesh
        const Stellar::Mesh cubeMesh(vertices, 24, indices, 36);

        // Background color used by glClear() on every frame
        glClearColor(0.0f, 0.07f, 0.12f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        Stellar::Camera camera (glm::vec3(0.0f, 0.0f, 5.0f));

        constexpr float moveDistance = 0.05f;

        // Main rendering loop
        while (!window.shouldClose()) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shaderProgram.use();
            shaderProgram.set_vec3("uColor", 1.0f, 1.0f, 1.0f);

            const auto time = static_cast<float>(glfwGetTime());

            constexpr auto modelMatrix = glm::mat4(1.0f);
            shaderProgram.set_mat4("uModel", glm::value_ptr(modelMatrix));

            if (glfwGetKey(window.get_window(), GLFW_KEY_W) == GLFW_PRESS)
            {
                camera.move_forward(moveDistance);
            }

            if (glfwGetKey(window.get_window(), GLFW_KEY_D) == GLFW_PRESS)
            {
                camera.move_right(moveDistance);
            }

            if (glfwGetKey(window.get_window(), GLFW_KEY_S) == GLFW_PRESS)
            {
                camera.move_forward(-moveDistance);
            }

            if (glfwGetKey(window.get_window(), GLFW_KEY_A) == GLFW_PRESS)
            {
                camera.move_right(-moveDistance);
            }

            const auto viewMatrix = camera.get_view_matrix();
            shaderProgram.set_mat4("uView", glm::value_ptr(viewMatrix));

            window.get_framebuffer_size(fbWidth, fbHeight);

            const float aspectRatio = (fbHeight > 0)
                ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight)
                : 1.0f;

            const auto projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
            shaderProgram.set_mat4("uProjection", glm::value_ptr(projectionMatrix));

            cubeMesh.draw();

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
