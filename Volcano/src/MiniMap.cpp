#include "MiniMap.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

MiniMap::MiniMap() 
    : viewportX(0), viewportY(0), viewportWidth(300), viewportHeight(300),
      cameraPosition(0.0f, 10000.0f, 0.0f),  
      lookAtCenter(0.0f, 0.0f, 0.0f),
      upVector(0.0f, 0.0f, -1.0f),  
      worldSize(10000.0f) {          
    
    updateCameraPosition();
}

void MiniMap::update(const glm::vec3& playerPos, const glm::vec3& volcanoPos) {
    lookAtCenter = glm::vec3(0.0f, 0.0f, 0.0f); 
    updateCameraPosition();
}

void MiniMap::render() {
}

void MiniMap::setWindowSize(int width, int height) {
    int minimapSize = std::min(width, height) / 4;
    viewportX = width - minimapSize - 20; 
    viewportY = height - minimapSize - 20; 
    viewportWidth = minimapSize;
    viewportHeight = minimapSize;
}

glm::mat4 MiniMap::getViewMatrix() const {
    return glm::lookAt(cameraPosition, lookAtCenter, upVector);
}

glm::mat4 MiniMap::getProjectionMatrix() const {
    float halfSize = worldSize / 2.0f;
    return glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1f, 20000.0f);
}

void MiniMap::getViewport(int& x, int& y, int& width, int& height) const {
    x = viewportX;
    y = viewportY;
    width = viewportWidth;
    height = viewportHeight;
}

void MiniMap::updateCameraPosition() {
    // Fixed position high above the world center
    cameraPosition = glm::vec3(0.0f, 10000.0f, 0.0f);
}