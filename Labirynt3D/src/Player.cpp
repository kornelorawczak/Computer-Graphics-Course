#include "Player.hpp"

// Defines Player parameters and has a simple updatePosition and checkGoalReached function

Player::Player() 
    : position(0.075f, 0.075f, 0.075f), 
      cameraOffset(0.0f, 0.0f, 0.0f), 
      radius(0.025f), 
      movementSpeed(2.0f),
      reachedGoal(false) {
}

void Player::moveForward(float deltaTime) {
    // Ingame
}

void Player::moveBackward(float deltaTime) {
    // Ingame
}

void Player::updatePosition(const glm::vec3& newPosition) {
    position = newPosition;
}

bool Player::checkGoalReached() const {
    glm::vec3 goal(1.0f, 1.0f, 1.0f);
    float distance = glm::length(position - goal);
    return distance < (radius + 0.15f); 
}