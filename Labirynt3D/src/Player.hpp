#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>

class Player {
public:
    Player();
    
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void updatePosition(const glm::vec3& newPosition);
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getCameraOffset() const { return cameraOffset; }
    float getRadius() const { return radius; }
    bool hasReachedGoal() const { return reachedGoal; }
    
    void setPosition(const glm::vec3& newPosition) { position = newPosition; }
    void setReachedGoal(bool reached) { reachedGoal = reached; }
    
    bool checkGoalReached() const;

private:
    glm::vec3 position;
    glm::vec3 cameraOffset;
    float radius;
    float movementSpeed;
    bool reachedGoal;
};

#endif