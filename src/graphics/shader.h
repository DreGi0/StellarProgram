/**
 * @file shader.h
 * @brief Shader class definition for compiling and managing OpenGL shader programs.
 * @author DreGi0
 * @date September 10th, 2026
 */

#pragma once
#include <string>
#include <unordered_map>
#include <GL/gl.h>

namespace Stellar {
    /**
     * @class Shader
     * @brief Encapsulates an OpenGL shader program, including compilation and uniform management.
     */
    class Shader {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        // Disable copy to avoid OpenGL handle duplication
        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        /**
         * @brief Move constructor. Transfers ownership of the OpenGL program handle.
         * @param other Instance the resources are moved from.
         */
        Shader(Shader&& other) noexcept;

        /**
         * @brief Move assignment operator.
         * @param other Instance the resources are moved from.
         * @return Reference to this instance.
         */
        Shader& operator=(Shader&& other) noexcept;

        /**
         * @brief Destructor. Deletes the shader program from the GPU.
         */
        ~Shader();

        /**
         * @brief Binds this shader program for subsequent rendering commands.
         */
        void use() const;

        // Note: Setters assume use() was called. OpenGL updates the uniform in the active program.

        /**
         * @brief Sets a floating-point uniform in the shader.
         * @param name Uniform name as declared in the shader source.
         * @param value Value to upload.
         */
        void set_float(const char* name, float value) const;

        /**
         * @brief Sets a 3-component vector uniform in the shader.
         * @param name Uniform name as declared in the shader source.
         * @param x,y,z Vector components.
         */
        void set_vec3(const std::string& name, float x, float y, float z) const;

        /**
         * @brief Sets a 4x4 matrix uniform in the shader.
         * @param name Uniform name as declared in the shader source.
         * @param matrixData Pointer to 16 floats in column-major order.
         */
        void set_mat4(const std::string& name, const float* matrixData) const;

        /**
         * @brief Retrieves the underlying OpenGL program ID.
         * @return The OpenGL program handle.
         */
        [[nodiscard]] GLuint get_id() const { return m_programId; }
    private:
        GLuint m_programId = 0;

        // Cache to avoid repeated string hashing and OpenGL lookups for uniforms
        mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;

        static GLuint compile_shader(GLenum type, const char* src);
        static std::string read_file(const std::string& path);

        static std::string get_shader_info_log(GLuint shader);
        static std::string get_program_info_log(GLuint program);

        GLint get_uniform_location(const std::string& name) const;
    };
} // Stellar

