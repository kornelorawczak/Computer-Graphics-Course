#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>
#include <vector>

struct CollisionSphere {
    glm::vec3 offset;
    float radius;
};

class Player {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    
    float yaw;
    float pitch;
    float speed;
    float sprintMultiplier;
    float slowMultiplier;
    float damping;
    
    bool isAlive;
    int lives;
    
    float bladeRotation;
    float bladeRotationSpeed;

    float flashlightForwardOffset;
    float flashlightUpOffset;
    
    std::vector<CollisionSphere> collisionSpheres;

    Player();
    void update(float dt);
    void processMovement(int forward, int right, int vertical, bool sprint, bool slow);
    void processMouseMovement(float xOffset, float yOffset);
    void applyForce(const glm::vec3& force);
    void resolveCollision(const glm::vec3& collisionPoint, const glm::vec3& normal);
    std::vector<glm::vec3> getCollisionSpherePositions() const;
    void updateVectors();
    void updateBladesOnly(float dt);

    float getBladeRotation() const { return bladeRotation; }

    glm::vec3 getFlashlightPosition() const { 
        return position  + (front * 3.0f);
    }

    glm::vec3 getFlashlightDirection() const { 
        return front; 
    }
    
    glm::vec3 getHorizontalFront() const {
        return glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    }
};

#endif