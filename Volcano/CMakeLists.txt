cmake_minimum_required(VERSION 3.10)
project(OpenGLHelicopterGame)  # Changed project name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

# Include directories
include_directories(include)
include_directories(glm-1.0.2)
include_directories(glfw-3.4/include)

# Add GLFW subdirectory
add_subdirectory(glfw-3.4)

# Source files
set(SRC_FILES
    src/main.cpp
    src/Camera.cpp
    src/Player.cpp
    src/InputHandler.cpp
    src/MeshGenerator.cpp
    src/Renderer.cpp
    src/BubbleManager.cpp
    src/ObstacleManager.cpp
    src/Collision.cpp
    src/ClosestPointTriangle.cpp
    src/glad.c
    src/MiniMap.cpp
    src/UI.cpp
)

# Create executable - use project name
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw opengl32)

# Copy shaders to binary directory
file(COPY shaders/ DESTINATION ${CMAKE_BINARY_DIR}/bin/shaders)

# Compiler specific settings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()