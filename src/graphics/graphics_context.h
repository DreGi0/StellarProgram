/**
 * @file graphics_context.h
 * @brief Loads the OpenGL functions (GLAD) for the current context.
 * @author DreGi0
 * @date September 25th, 2026
 */

#pragma once

namespace Stellar
{
    /**
     * @class GraphicsContext
     * @brief Loads OpenGL through GLAD and prints the GPU / driver info.
     *
     * Must be created after the Window (which makes the context current)
     * and before anything that calls OpenGL. In Debug builds it also enables GL debug output.
     */
    class GraphicsContext
    {
        public:
        /**
         * @brief Loads the OpenGL functions for the current context.
         * @throws std::runtime_error If GLAD cannot load OpenGL.
         */
        GraphicsContext();
    };
} // Stellar
