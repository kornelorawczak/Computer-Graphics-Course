#include "Collision.hpp"
#include "ClosestPointTriangle.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

bool CollisionSystem::sphereSphereCollision(const glm::vec3& pos1, float radius1, 
                                           const glm::vec3& pos2, float radius2) {
    float distance = glm::length(pos1 - pos2);
    return distance < (radius1 + radius2);
}

bool CollisionSystem::sphereTriangleCollision(const glm::vec3& spherePos, float sphereRadius,
                                             const Triangle& triangle, glm::vec3& collisionPoint) {
    glm::vec3 closestPoint = closestPointTriangle(spherePos, triangle.v0, triangle.v1, triangle.v2);
    
    float distance = glm::length(spherePos - closestPoint);
    if (distance < sphereRadius) {
        collisionPoint = closestPoint;
        return true;
    }
    
    return false;
}

glm::vec3 CollisionSystem::closestPointOnMesh(const glm::vec3& point, const std::vector<float>& vertices,
                                             const std::vector<unsigned int>& indices) {
    glm::vec3 closestPoint(0.0f);
    float closestDistance = std::numeric_limits<float>::max();
    
    auto triangles = extractTrianglesFromMesh(vertices, indices);
    
    for (const auto& triangle : triangles) {
        glm::vec3 triangleClosest = closestPointTriangle(point, triangle.v0, triangle.v1, triangle.v2);
        float distance = glm::length(point - triangleClosest);
        
        if (distance < closestDistance) {
            closestDistance = distance;
            closestPoint = triangleClosest;
        }
    }
    
    return closestPoint;
}

std::vector<Triangle> CollisionSystem::extractTrianglesFromMesh(const std::vector<float>& vertices,
                                                               const std::vector<unsigned int>& indices) {
    std::vector<Triangle> triangles;
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int idx0 = indices[i] * 6;
        unsigned int idx1 = indices[i + 1] * 6;
        unsigned int idx2 = indices[i + 2] * 6;
        
        Triangle triangle;
        triangle.v0 = glm::vec3(vertices[idx0], vertices[idx0 + 1], vertices[idx0 + 2]);
        triangle.v1 = glm::vec3(vertices[idx1], vertices[idx1 + 1], vertices[idx1 + 2]);
        triangle.v2 = glm::vec3(vertices[idx2], vertices[idx2 + 1], vertices[idx2 + 2]);
        
        glm::vec3 edge1 = triangle.v1 - triangle.v0;
        glm::vec3 edge2 = triangle.v2 - triangle.v0;
        triangle.normal = glm::normalize(glm::cross(edge1, edge2));
        
        triangles.push_back(triangle);
    }
    
    return triangles;
}