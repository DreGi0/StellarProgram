/**
 * @file shader.cpp
 * @brief Implementation of the Shader class, compilation, and uniform management.
 * @author DreGi0
 * @date September 10th, 2026
 */

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <glad/glad.h>

#include "shader.h"

namespace Stellar {
    Shader::Shader(const  std::string& vertexPath, const std::string& fragmentPath) {
        const std::string vertexSrc = read_file(vertexPath);
        const std::string fragmentSrc = read_file(fragmentPath);

        const GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertexSrc.c_str());
        if (!vertex) {
            throw std::runtime_error("[Shader] Failed to compile vertex shader: " + vertexPath);
        }

        const GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc.c_str());
        if (!fragment) {
            glDeleteShader(vertex);
            throw std::runtime_error("[Shader] Failed to compile vertex shader: " + fragmentPath);
        }

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        // Cleanup individual shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            const std::string log = get_program_info_log(program);
            glDeleteProgram(program);

            throw std::runtime_error("[Shader] Link failed:\n" + log);
        }

        m_programId = program;
    }

    Shader::Shader(Shader &&other) noexcept :
    m_programId(other.m_programId),
    m_uniformLocationCache(std::move(other.m_uniformLocationCache)){
        other.m_programId = 0;
        other.m_uniformLocationCache.clear();
    }

    Shader & Shader::operator=(Shader &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        glDeleteProgram(m_programId);

        // Gather
        m_programId = other.m_programId;
        m_uniformLocationCache = std::move(other.m_uniformLocationCache);

        // Cleanup
        other.m_programId = 0;
        other.m_uniformLocationCache.clear();

        return *this;
    }

    Shader::~Shader() {
        glDeleteProgram(m_programId);
    }

    void Shader::use() const {
        glUseProgram(m_programId);
    }

    void Shader::set_float(const char *name, float value) const {
        glUniform1f(get_uniform_location(name), value);
    }

    void Shader::set_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(get_uniform_location(name), x, y, z);
    }

    void Shader::set_mat4(const std::string &name, const float *matrixData) const {
        glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, matrixData);
    }

    GLuint Shader::compile_shader(const GLenum type, const char *src) {
        const GLuint shader = glCreateShader(type);

        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        // Errors validation
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            const char* typeName = (type == GL_VERTEX_SHADER ? "vertex" : "fragment");
            fprintf(stderr, "[Shader] failed to compile %s shader:\n%s\n",
                typeName, get_shader_info_log(shader).c_str());
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    std::string Shader::read_file(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file at path: " + path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }

    std::string Shader::get_shader_info_log(GLuint shader) {
        GLint length = 0;

        // Get warning message length from OpenGL
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        if (length <= 1) return {}; // empty log

        // Pre-allocate string based on the actual log format length
        std::string log(static_cast<size_t>(length), '\0');
        glGetShaderInfoLog(shader, length, nullptr, log.data());

        // Remove null terminator
        log.pop_back();
        return log;
    }

    // Same shape as getShaderInfoLog but querying program state.
    // Note: kept duplicated on purpose since the OpenGL calls differ
    // (glGetShaderiv vs glGetProgramiv), so abstracting it costs more than repeating it.

    std::string Shader::get_program_info_log(const GLuint program) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        if (length <= 1) return {}; // empty log x2 :)

        std::string log(static_cast<size_t>(length), '\0');
        glGetProgramInfoLog(program, length, nullptr, log.data());
        log.pop_back();
        return log;
    }

    GLint Shader::get_uniform_location(const std::string &name) const {
        if (const auto it = m_uniformLocationCache.find(name);
            it != m_uniformLocationCache.end()) {
            return it->second;
        }

        const GLint location = glGetUniformLocation(m_programId, name.c_str());
        m_uniformLocationCache[name] = location;
        return location;
    }
} // Stellar