cmake_minimum_required(VERSION 3.28)

project(StellarProgram)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(StellarEngine STATIC
        src/core/window.cpp
        src/core/window.h
        src/graphics/shader.cpp
        src/graphics/shader.h
        src/core/paths.cpp
        src/core/paths.h
        src/graphics/mesh.cpp
        src/graphics/mesh.h
        src/graphics/gl_debug.cpp
        src/graphics/gl_debug.h
        src/graphics/camera.cpp
        src/graphics/camera.h
        src/graphics/graphics_context.cpp
        src/graphics/graphics_context.h
        src/core/application.cpp
        src/core/application.h)

add_executable(StellarProgram src/main.cpp)
target_link_libraries(StellarProgram PRIVATE StellarEngine)

target_include_directories(StellarEngine PUBLIC src)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(StellarEngine PUBLIC glfw)
target_link_libraries(StellarEngine PUBLIC glad::glad)
target_link_libraries(StellarEngine PUBLIC glm::glm)

target_compile_definitions(StellarEngine PUBLIC GLFW_INCLUDE_NONE)
target_compile_definitions(StellarEngine PRIVATE $<$<CONFIG:Debug>:STELLAR_DEBUG>)

add_custom_command(TARGET StellarProgram POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:StellarProgram>/assets
        COMMENT "Copying assets next to the executable")


target_compile_options(StellarEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

target_compile_options(StellarProgram PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(StellarTests tests/sanity_tests.cpp
        tests/camera_tests.cpp)
target_link_libraries(StellarTests PRIVATE StellarEngine Catch2::Catch2WithMain)

enable_testing()
include(Catch)
catch_discover_tests(StellarTests)
