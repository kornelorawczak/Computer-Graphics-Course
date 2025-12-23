#ifndef COINMANAGER_HPP
#define COINMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <random>

struct Coin {
    glm::vec3 position;
    bool collected;
    float radius;
};

class CoinManager {
public:
    CoinManager();
    
    void generateCoins(int count, const std::vector<glm::vec3>& obstaclePositions, 
                      float obstacleRadius, int gridSize, int seed = 0);
    void checkPlayerCollision(glm::vec3 playerPosition, float playerRadius, int& score);
    const std::vector<Coin>& getCoins() const { return coins; }
    int getCollectedCount() const { return collectedCount; }

private:
    std::vector<Coin> coins;
    int collectedCount;
    
    bool isValidCoinPosition(const glm::vec3& position, const std::vector<glm::vec3>& obstaclePositions,
                           float obstacleRadius, float coinRadius);
};

#endif