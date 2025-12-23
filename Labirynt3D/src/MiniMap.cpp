#include "MiniMap.hpp"
#include <glm/gtc/matrix_transform.hpp>

MiniMap::MiniMap() 
    : currentViewSide(ViewSide::FRONT),
      viewportX(850), viewportY(850), viewportWidth(300), viewportHeight(300) {
    updateCameraPosition();
}

void MiniMap::update(float deltaTime) {
}

void MiniMap::switchViewSide(ViewSide newSide) {
    currentViewSide = newSide;
    updateCameraPosition();
}

void MiniMap::cycleViewSide() {
    switch (currentViewSide) {
        case ViewSide::FRONT: switchViewSide(ViewSide::FRONT); break;
        case ViewSide::SIDE: switchViewSide(ViewSide::SIDE); break;
    }
}

void MiniMap::updateCameraPosition() {
    glm::vec3 position;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    switch (currentViewSide) {
        case ViewSide::FRONT:
            position = frontPosition;
            break;
        case ViewSide::SIDE:
            position = sidePosition;
            break;
    }
    
    camera.setPosition(position);
    camera.setYaw(0.0f);
    camera.setPitch(0.0f);
}

glm::mat4 MiniMap::getViewMatrix() const {
    return glm::lookAt(camera.getPosition(), lookAtCenter, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 MiniMap::getProjectionMatrix() const {
    // Orthographic projection for minimap to avoid perspective distortion
    return glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 10.0f);
}

void MiniMap::setViewport(int x, int y, int width, int height) {
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
}

void MiniMap::getViewport(int& x, int& y, int& width, int& height) const {
    x = viewportX;
    y = viewportY;
    width = viewportWidth;
    height = viewportHeight;
}