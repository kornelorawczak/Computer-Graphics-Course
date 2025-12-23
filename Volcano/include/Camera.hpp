#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMode {
    THIRD_PERSON,
    FIRST_PERSON
};

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f));
    
    void update(float dt);
    void processMouseScroll(float yOffset);
    
    void setPosition(const glm::vec3& position);
    void setFront(const glm::vec3& front);
    void setMode(CameraMode mode);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
    float getFOV() const;
    
    void updateThirdPerson(const glm::vec3& targetPos, const glm::vec3& targetFront, const glm::vec3& targetUp);
    void updateFirstPerson(const glm::vec3& targetPos, const glm::vec3& targetFront, const glm::vec3& targetUp);
    
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    float movementSpeed;
    float zoom;
    
    CameraMode currentMode;

private:
    void updateCameraVectors();
};

#endif