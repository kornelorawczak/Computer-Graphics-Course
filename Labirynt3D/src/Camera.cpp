#include "Camera.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>  

Camera::Camera(glm::vec3 position) 
    : position(position), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), 
      yaw(45.0f), pitch(0.0f), 
      movementSpeed(2.0f), mouseSensitivity(0.05f), zoom(60.0f) {
    updateVectors();
}

void Camera::updateVectors() {
    // Calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    // re-calculate the right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(zoom), aspectRatio, 0.01f, 100.0f);
}

void Camera::processMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateVectors();
}