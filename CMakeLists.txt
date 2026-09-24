cmake_minimum_required(VERSION 3.28)

project(StellarProgram)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(StellarProgram src/main.cpp
        src/core/window.cpp
        src/core/window.h
        src/graphics/shader.cpp
        src/graphics/shader.h
        src/core/paths.cpp
        src/core/paths.h
        src/graphics/mesh.cpp
        src/graphics/mesh.h)

add_custom_command(TARGET StellarProgram POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:StellarProgram>/assets
        COMMENT "Copying assets next to the executable")

target_compile_options(StellarProgram PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(StellarProgram PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(StellarProgram PRIVATE glad::glad)

find_package(glm CONFIG REQUIRED)
target_link_libraries(StellarProgram PRIVATE glm::glm)
