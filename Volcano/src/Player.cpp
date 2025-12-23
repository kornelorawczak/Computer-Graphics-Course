#include "Player.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

Player::Player() {
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = worldUp;
    
    yaw = -90.0f;
    pitch = 0.0f;
    
    speed = 15.0f;
    sprintMultiplier = 2.0f;
    slowMultiplier = 0.5f;
    damping = 0.95f;
    
    isAlive = true;
    lives = 3;
    
    bladeRotation = 0.0f;
    bladeRotationSpeed = 720.0f; 

    flashlightForwardOffset = 3.0f; 
    flashlightUpOffset = 0.0f;
    
    collisionSpheres = {
        {glm::vec3(0.0f, 0.0f, 0.0f), 2.0f},  
    };
    
    updateVectors();
}

void Player::updateBladesOnly(float dt) {
    bladeRotation += bladeRotationSpeed * dt;
    if (bladeRotation >= 360.0f) {
        bladeRotation -= 360.0f;
    }
}

void Player::update(float dt) {
    velocity *= damping;
    
    position += velocity * dt;
    
    bladeRotation += bladeRotationSpeed * dt;
    if (bladeRotation >= 360.0f) {
        bladeRotation -= 360.0f;
    }
    
    if (position.y < 10.0f) {
        position.y = 10.0f;
        velocity.y = 0.0f;
    }
}

void Player::processMovement(int forward, int right, int vertical, bool sprint, bool slow) {
    glm::vec3 moveDirection(0.0f);
    
    if (forward != 0) {
        moveDirection += front * (float)forward;
    }
    if (right != 0) {
        moveDirection += this->right * (float)right;
    }
    
    if (vertical != 0) {
        moveDirection.y += vertical;
    }
    
    if (glm::length(glm::vec3(moveDirection.x, 0.0f, moveDirection.z)) > 0.0f) {
        glm::vec3 horizontalMove = glm::normalize(glm::vec3(moveDirection.x, 0.0f, moveDirection.z));
        moveDirection.x = horizontalMove.x;
        moveDirection.z = horizontalMove.z;
    }
    
    float currentSpeed = speed;
    if (sprint) currentSpeed *= sprintMultiplier;
    if (slow) currentSpeed *= slowMultiplier;
    
    applyForce(moveDirection * currentSpeed);
}

void Player::processMouseMovement(float xOffset, float yOffset) {
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateVectors();
}

void Player::applyForce(const glm::vec3& force) {
    acceleration += force;
    velocity += acceleration;
    acceleration = glm::vec3(0.0f);
}

void Player::resolveCollision(const glm::vec3& collisionPoint, const glm::vec3& normal) {
    glm::vec3 pushDirection = glm::normalize(position - collisionPoint);
    position = collisionPoint + pushDirection * 5.0f; 
    
    float dotProduct = glm::dot(velocity, normal);
    if (dotProduct < 0) {
        velocity -= normal * dotProduct * 1.5f;
    }
}

std::vector<glm::vec3> Player::getCollisionSpherePositions() const {
    std::vector<glm::vec3> positions;
    for (const auto& sphere : collisionSpheres) {
        positions.push_back(position + sphere.offset);
    }
    return positions;
}

void Player::updateVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}