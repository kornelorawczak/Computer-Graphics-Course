#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    
    void updateVectors();
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;
    
    void processMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getFront() const { return front; }
    glm::vec3 getRight() const { return right; }
    glm::vec3 getUp() const { return up; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    
    void setPosition(const glm::vec3& newPosition) { position = newPosition; }
    void setYaw(float newYaw) { yaw = newYaw; updateVectors(); }
    void setPitch(float newPitch) { pitch = newPitch; updateVectors(); }

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;
    
    float yaw;
    float pitch;
    
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
};

#endif