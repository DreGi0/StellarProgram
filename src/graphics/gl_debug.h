/**
 * @file gl_debug.h
 * @brief OpenGL 4.3+ debug output setup.
 * @author DreGi0
 * @date September 12th, 2026
 */

#pragma once

namespace Stellar {
    /**
     * @brief Enables the OpenGL debug message callback (requires OpenGL 4.3+).
     * @note No-op with a warning on stderr if the context is older than 4.3.
     */
    void enableDebugOutput();
} // Stellar