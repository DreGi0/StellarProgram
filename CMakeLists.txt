cmake_minimum_required(VERSION 3.28)
project(StellarProgram)

# LANGUAGE STANDARD
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# WARNING FLAGS
add_library(stellar_warnings INTERFACE)
target_compile_options(stellar_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
)

# DEPENDENCIES
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Catch2 3 CONFIG REQUIRED)

# ENGINE LIBRARY
add_library(StellarEngine STATIC
        src/core/window.cpp
        src/core/window.h
        src/core/paths.cpp
        src/core/paths.h
        src/core/application.cpp
        src/core/application.h
        src/core/input.cpp
        src/core/input.h
        src/graphics/shader.cpp
        src/graphics/shader.h
        src/graphics/mesh.cpp
        src/graphics/mesh.h
        src/graphics/gl_debug.cpp
        src/graphics/gl_debug.h
        src/graphics/camera.cpp
        src/graphics/camera.h
        src/graphics/graphics_context.cpp
        src/graphics/graphics_context.h
        src/ui/debug_overlay.cpp
        src/ui/debug_overlay.h
        src/physics/orbit.cpp
        src/physics/orbit.h
        src/game/world.cpp
        src/game/world.h
        src/graphics/primitives.cpp
        src/graphics/primitives.h
        src/graphics/orbit_camera.cpp
        src/graphics/orbit_camera.h
)

target_include_directories(StellarEngine PUBLIC src)

target_link_libraries(StellarEngine
        PUBLIC
        glfw
        glad::glad
        glm::glm
        PRIVATE
        imgui::imgui
        stellar_warnings
)

target_compile_definitions(StellarEngine
        PUBLIC GLFW_INCLUDE_NONE
        PRIVATE $<$<CONFIG:Debug>:STELLAR_DEBUG>
)

# MAIN EXECUTABLE
add_executable(StellarProgram src/main.cpp)
target_link_libraries(StellarProgram PRIVATE StellarEngine stellar_warnings)

add_custom_command(TARGET StellarProgram POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:StellarProgram>/assets
        COMMENT "Copying assets next to the executable"
)

# TESTS
enable_testing()

add_executable(StellarTests
        tests/sanity_tests.cpp
        tests/camera_tests.cpp
        tests/orbit_tests.cpp
        tests/world_tests.cpp
        tests/primitives_tests.cpp
)
target_link_libraries(StellarTests PRIVATE StellarEngine Catch2::Catch2WithMain stellar_warnings)

include(Catch)
catch_discover_tests(StellarTests)
