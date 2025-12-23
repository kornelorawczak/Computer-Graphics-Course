#include "Camera.hpp"
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 position) 
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
      movementSpeed(25.0f), zoom(45.0f) {
    this->position = position;
    updateCameraVectors();
    currentMode = THIRD_PERSON; 
}

void Camera::update(float dt) {
}

void Camera::updateThirdPerson(const glm::vec3& targetPos, const glm::vec3& targetFront, const glm::vec3& targetUp) {
    glm::vec3 offset = -targetFront * 15.0f + glm::vec3(0.0f, 8.0f, 0.0f);
    position = targetPos + offset;
    
    front = glm::normalize(targetPos - position);
    up = targetUp;
    updateCameraVectors();
}

void Camera::updateFirstPerson(const glm::vec3& targetPos, const glm::vec3& targetFront, const glm::vec3& targetUp) {
    position = targetPos;
    
    front = targetFront;
    up = targetUp;
    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    zoom -= (float)yOffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 90.0f)
        zoom = 90.0f;
}

void Camera::setPosition(const glm::vec3& position) {
    this->position = position;
}

void Camera::setFront(const glm::vec3& front) {
    this->front = front;
    updateCameraVectors();
}

void Camera::setMode(CameraMode mode) {
    currentMode = mode;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(zoom), 1200.0f / 800.0f, 0.1f, 20000.0f);
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::vec3 Camera::getFront() const {
    return front;
}

glm::vec3 Camera::getUp() const {
    return up;
}

float Camera::getFOV() const {
    return zoom;
}

void Camera::updateCameraVectors() {
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}