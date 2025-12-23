#ifndef OBSTACLEMANAGER_HPP
#define OBSTACLEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <random>

struct Obstacle {
    glm::vec3 position;
    glm::vec3 color;
    glm::mat4 transform;
    std::vector<glm::vec3> triangleVertices;
    float boundingRadius;

    glm::vec3 rotationAxis;
    float rotationSpeed;
    float currentRotation;
};

class ObstacleManager {
public:
    ObstacleManager();
    
    void generateObstacles(int gridSize, int seed = 0);
    void updateObstacles(float deltaTime);
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    
    int getGridSize() const { return gridSize; }
    int getSeed() const { return seed; }
    float getSpacing() const { return spacing; }
    float getObstacleRadius() const { return obstacleRadius; }

private:
    std::vector<Obstacle> obstacles;
    int gridSize;
    int seed;
    float spacing;
    float obstacleRadius;
    
    void generateTriangleVertices(Obstacle& obstacle);
};

#endif