#ifndef OBSTACLEMANAGER_HPP
#define OBSTACLEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

struct Mesh;

struct Obstacle {
    glm::vec3 position;
    float radius;
    bool active;
};

class ObstacleManager {
public:
    ObstacleManager();
    
    void initializeVolcanoObstacles(const Mesh& volcanoMesh);
    void update(float dt);
    
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
    
private:
    std::vector<Obstacle> obstacles;
};

#endif