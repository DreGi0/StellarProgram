/**
 * @file paths.h
 * @brief Utility functions for resolving file paths across the engine.
 * @author DreGi0
 * @date September 11th, 2026
 */

#pragma once

#include <string>

namespace Stellar {
    /**
     * @brief Resolves a relative path against the asset root directory.
     *
     * @note The asset root is defined as the 'assets' folder located in the same
     * directory as the executable.
     *
     * @param relative The relative path to the asset (e.g., "textures/ship.png").
     * @return The absolute path to the asset as a string.
     */
    std::string asset_path(const std::string& relative);
} // Stellar
