#include "ObstacleManager.hpp"
#include "MeshGenerator.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>

// generates randomly rotated obstacles 

ObstacleManager::ObstacleManager() 
    : gridSize(5), seed(0), spacing(0.2f), obstacleRadius(0.13f) {
}

void ObstacleManager::generateObstacles(int gridSize, int seed) {
    this->gridSize = gridSize;
    this->seed = seed;
    
    // Calculate spacing and obstacle radius
    spacing = (gridSize > 1) ? 1.0f / (gridSize - 1) : 1.0f;
    obstacleRadius = (gridSize > 1) ? spacing * 0.5f : 0.1f;
    
    obstacles.clear();
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(0.0f, 360.0f);
    std::uniform_real_distribution<float> speedDist(0.3f, 0.7f);
    
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            for (int k = 0; k < gridSize; k++) {
                if (!(i==0 && j==0 && k==0)) {
                    Obstacle obstacle;
                    obstacle.position = glm::vec3(
                        i * spacing,
                        j * spacing, 
                        k * spacing
                    );
                    
                    // Color based on position
                    obstacle.color.r = 0.7f * obstacle.position.x + 0.3f;
                    obstacle.color.g = 0.7f * obstacle.position.y + 0.3f;
                    obstacle.color.b = 0.7f * obstacle.position.z + 0.3f;
                    
                    // Random rotation
                    float angleX = dist(rng);
                    float angleY = dist(rng);
                    float angleZ = dist(rng);

                    // Random rotation axis for continuous rotation
                    obstacle.rotationAxis = glm::sphericalRand(1.0f); 
                    obstacle.rotationSpeed = speedDist(rng); 
                    obstacle.currentRotation = 0.0f;
                    
                    obstacle.transform = glm::mat4(1.0f);
                    obstacle.transform = glm::translate(obstacle.transform, obstacle.position);
                    obstacle.transform = glm::rotate(obstacle.transform, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
                    obstacle.transform = glm::rotate(obstacle.transform, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
                    obstacle.transform = glm::rotate(obstacle.transform, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));
                    obstacle.transform = glm::scale(obstacle.transform, glm::vec3(obstacleRadius));
                    
                    obstacle.boundingRadius = obstacleRadius * 1.5f; // Conservative bounding sphere
                    
                    generateTriangleVertices(obstacle);
                    obstacles.push_back(obstacle);
                }
            }
        }
    }
}

void ObstacleManager::updateObstacles(float deltaTime) {
    for (auto& obstacle : obstacles) {
        // Update rotation angle based on time and speed
        obstacle.currentRotation += obstacle.rotationSpeed * deltaTime;
        
        if (obstacle.currentRotation > 2 * 3.14159f) {
            obstacle.currentRotation -= 2 * 3.14159f;
        }
        
        // Rebuild transform with updated rotation
        obstacle.transform = glm::mat4(1.0f);
        obstacle.transform = glm::translate(obstacle.transform, obstacle.position);
        obstacle.transform = glm::rotate(obstacle.transform, obstacle.currentRotation, obstacle.rotationAxis);
        obstacle.transform = glm::scale(obstacle.transform, glm::vec3(obstacleRadius));
        
        generateTriangleVertices(obstacle);
    }
}

void ObstacleManager::generateTriangleVertices(Obstacle& obstacle) {
    Mesh octahedron = MeshGenerator::generateOctahedron();
    obstacle.triangleVertices.clear();
    
    for (size_t i = 0; i < octahedron.indices.size(); i += 3) {
        glm::vec3 a = octahedron.vertices[octahedron.indices[i]].position;
        glm::vec3 b = octahedron.vertices[octahedron.indices[i+1]].position;
        glm::vec3 c = octahedron.vertices[octahedron.indices[i+2]].position;
        
        // Transform vertices to world space
        glm::vec4 worldA = obstacle.transform * glm::vec4(a, 1.0f);
        glm::vec4 worldB = obstacle.transform * glm::vec4(b, 1.0f);
        glm::vec4 worldC = obstacle.transform * glm::vec4(c, 1.0f);
        
        obstacle.triangleVertices.push_back(glm::vec3(worldA));
        obstacle.triangleVertices.push_back(glm::vec3(worldB));
        obstacle.triangleVertices.push_back(glm::vec3(worldC));
    }
}