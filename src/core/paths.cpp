/**
 * @file paths.cpp
 * @brief Implementation of path resolution utilities.
 * @author DreGi0
 * @date September 11th, 2026
 */

#include "paths.h"
#include "filesystem"
#ifdef _WIN32
#include <windows.h>
#endif

namespace Stellar {
    /*
     * If you're dumb like me, the workflow is like this conversation between methods:
     * assetpath(): Hey executableDir()! I need to know the path to the exe since assets is at the same level.
     * executableDir(): Okay, let me see with Linux. Hey! what's the path to the exe in this environment.
     * Linux: It's 'path/to/project/build/Apogee'.
     * executableDir(): Thanks. assetpath(), the path is 'path/to/project/build/Apogee'.
     * assetpath(): Thanks. So my path it actually is 'path/to/project/build/assets'.
     *
     * PS: I don't know why I found this one kind of hard to comprehend even though it's a pretty simple concept.
     */

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
