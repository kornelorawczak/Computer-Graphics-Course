cmake_minimum_required(VERSION 3.10)
project(OpenGLTriangle)

set(CMAKE_CXX_STANDARD 17)

# --- Katalogi nagłówków ---
include_directories(include)
include_directories(glm-1.0.2)

# --- Źródła projektu ---
add_executable(OpenGLTriangle
    src/main.cpp
    src/glad.c
)

# --- Dodanie lokalnego GLFW ---
add_subdirectory(glfw-3.4)
target_link_libraries(OpenGLTriangle glfw)

# --- Windows: link do OpenGL ---
if (WIN32)
    target_link_libraries(OpenGLTriangle opengl32)
endif()
