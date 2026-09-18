/**
 * @file paths.cpp
 * @brief Implementation of path resolution utilities.
 * @author DreGi0
 * @date September 11th, 2026
 */

#include "paths.h"
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

namespace Stellar {
    namespace fs = std::filesystem;

    /// Returns the directory containing the running executable.
    static fs::path executable_dir() {
#ifdef _WIN32
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        return fs::path(path).parent_path();
#else
        std::error_code err;
        const fs::path exe = fs::read_symlink("/proc/self/exe", err);
        if (err) { return fs::current_path(); }
        return exe.parent_path();
#endif
    }

    std::string asset_path(const std::string &relative) {
        static const fs::path root = executable_dir() / "assets";
        return (root / relative).string();
    }
} // Stellar
