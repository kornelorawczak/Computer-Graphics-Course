#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <glm/glm.hpp>
#include <vector>
#include "ClosestPointTriangle.h"
struct Obstacle;

class Collision {
public:
    static bool checkSphereTriangleCollision(const glm::vec3& sphereCenter, float radius,
                                           const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    
    static bool checkSphereObstacleCollision(const glm::vec3& sphereCenter, float radius,
                                           const Obstacle& obstacle);
    
    static bool checkWorldBoundaries(const glm::vec3& position, float radius, glm::vec3& newPosition);
    
};

#endif