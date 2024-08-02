cmake_minimum_required(VERSION 3.10)

project(PSim)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Add source files
file(GLOB_RECURSE SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/imgui/*.cpp"
)

# Add header files
file(GLOB_RECURSE HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/glm/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/vendor/imgui/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} libs/glad/src/glad.c
        src/ParticleUtils.cpp
        include/UniformBuffer.h
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/GLFW/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vendor
)

# Define GLFW library path
set(GLFW_LIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/GLFW/lib-mingw-w64/libglfw3.a")

# Check if library exists
if(NOT EXISTS ${GLFW_LIB})
    message(FATAL_ERROR "GLFW library not found at ${GLFW_LIB}")
endif()

# Enable compiler warnings and debug information
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g")

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLFW_LIB}
        opengl32
        user32
        gdi32
        shell32
)

# Add compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)

# Custom command to copy shader files
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/res/shaders
        COMMENT "Copying shader files..."
)

# Custom target to copy shaders when running the project
add_custom_target(copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res/shaders ${CMAKE_CURRENT_BINARY_DIR}/res/shaders
        DEPENDS ${SHADER_FILES}
        COMMENT "Copying shader files..."
)

# Ensure shaders are copied before building or running
add_dependencies(${PROJECT_NAME} copy_shaders)
