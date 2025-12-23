#include "ObstacleManager.hpp"
#include "MeshGenerator.hpp"  // Add this include
#include <random>

ObstacleManager::ObstacleManager() {
}

void ObstacleManager::initializeVolcanoObstacles(const Mesh& volcanoMesh) {
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist(-1500.0f, 1500.0f);
    std::uniform_real_distribution<float> heightDist(100.0f, 2500.0f);
    std::uniform_real_distribution<float> radiusDist(10.0f, 50.0f);
    
    for (int i = 0; i < 20; i++) {
        Obstacle obstacle;
        obstacle.position = glm::vec3(dist(rng), heightDist(rng), dist(rng));
        obstacle.radius = radiusDist(rng);
        obstacle.active = true;
        obstacles.push_back(obstacle);
    }
}

void ObstacleManager::update(float dt) {
}