#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <glm/glm.hpp>
#include <vector>

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
};

class CollisionSystem {
public:
    static bool sphereSphereCollision(const glm::vec3& pos1, float radius1, 
                                     const glm::vec3& pos2, float radius2);
    
    static bool sphereTriangleCollision(const glm::vec3& spherePos, float sphereRadius,
                                       const Triangle& triangle, glm::vec3& collisionPoint);
    
    static glm::vec3 closestPointOnMesh(const glm::vec3& point, const std::vector<float>& vertices,
                                       const std::vector<unsigned int>& indices);
    
    static std::vector<Triangle> extractTrianglesFromMesh(const std::vector<float>& vertices,
                                                         const std::vector<unsigned int>& indices);
};

#endif