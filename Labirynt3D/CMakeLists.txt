cmake_minimum_required(VERSION 3.10)
project(OpenGLMaze)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(include)
include_directories(glm-1.0.2)

# Add GLFW subdirectory
add_subdirectory(glfw-3.4)

# Source files
set(SRC_FILES
    src/main.cpp
    src/Camera.cpp
    src/Player.cpp
    src/MeshGenerator.cpp
    src/ObstacleManager.cpp
    src/Renderer.cpp
    src/InputHandler.cpp
    src/Collision.cpp
    src/ClosestPointTriangle.cpp
    src/glad.c
    src/MiniMap.cpp
    src/CoinManager.cpp
)

# Create executable
add_executable(OpenGLMaze ${SRC_FILES})

# Link libraries
target_link_libraries(OpenGLMaze glfw opengl32)

# Copy shaders to build directory
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)