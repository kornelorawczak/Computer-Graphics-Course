#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "Collision.hpp"
#include "ObstacleManager.hpp"
#include <algorithm>

bool Collision::checkSphereTriangleCollision(const glm::vec3& sphereCenter, float radius,
                                           const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 closest = closestPointTriangle(sphereCenter, a, b, c);
    float distance = glm::length(sphereCenter - closest);
    return distance < radius;
}

bool Collision::checkSphereObstacleCollision(const glm::vec3& sphereCenter, float radius,
                                           const Obstacle& obstacle) {
    // First check bounding sphere for early out
    float distanceToCenter = glm::length(sphereCenter - obstacle.position);
    if (distanceToCenter > (radius + obstacle.boundingRadius)) {
        return false;
    }
    
    // Check all triangles
    for (size_t i = 0; i < obstacle.triangleVertices.size(); i += 3) {
        if (checkSphereTriangleCollision(sphereCenter, radius,
                                        obstacle.triangleVertices[i],
                                        obstacle.triangleVertices[i+1],
                                        obstacle.triangleVertices[i+2])) {
            return true;
        }
    }
    
    return false;
}

bool Collision::checkWorldBoundaries(const glm::vec3& position, float radius, glm::vec3& newPosition) {
    newPosition = position;
    bool collision = false;
    
    // Check each axis
    if (position.x - radius < 0.0f) {
        newPosition.x = radius;
        collision = true;
    } else if (position.x + radius > 1.0f) {
        newPosition.x = 1.0f - radius;
        collision = true;
    }
    
    if (position.y - radius < 0.0f) {
        newPosition.y = radius;
        collision = true;
    } else if (position.y + radius > 1.0f) {
        newPosition.y = 1.0f - radius;
        collision = true;
    }
    
    if (position.z - radius < 0.0f) {
        newPosition.z = radius;
        collision = true;
    } else if (position.z + radius > 1.0f) {
        newPosition.z = 1.0f - radius;
        collision = true;
    }
    
    return collision;
}