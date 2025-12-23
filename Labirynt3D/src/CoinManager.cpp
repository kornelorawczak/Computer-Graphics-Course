#include "CoinManager.hpp"
#include <algorithm>
#include <glm/gtc/constants.hpp>
#include <iostream>

CoinManager::CoinManager() 
    : collectedCount(0) {
}

void CoinManager::generateCoins(int count, const std::vector<glm::vec3>& obstaclePositions,
                               float obstacleRadius, int gridSize, int seed) {
    coins.clear();
    collectedCount = 0;
    
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> posDist(0.1f, 0.9f); 
    std::uniform_real_distribution<float> heightDist(0.2f, 0.8f); 
    
    float coinRadius = 0.015f; 
    
    int attempts = 0;
    int maxAttempts = count * 100; // Prevent infinite loop
    
    while (coins.size() < count && attempts < maxAttempts) {
        glm::vec3 candidatePos(
            posDist(rng),
            heightDist(rng), 
            posDist(rng)
        );
        
        // Check if this position is valid (not too close to obstacles)
        if (isValidCoinPosition(candidatePos, obstaclePositions, obstacleRadius, coinRadius)) {
            Coin coin;
            coin.position = candidatePos;
            coin.collected = false;
            coin.radius = coinRadius;
            coins.push_back(coin);
        }
        
        attempts++;
    }
    
}

void CoinManager::checkPlayerCollision(glm::vec3 playerPosition, float playerRadius, int& score) {
    for (auto& coin : coins) {
        if (!coin.collected) {
            float distance = glm::length(playerPosition - coin.position);
            float collisionDistance = playerRadius + coin.radius;
            
            if (distance < collisionDistance) {
                coin.collected = true;
                collectedCount++;
                score++;
                std::cout << "Coin collected! Score: " << score << std::endl;
            }
        }
    }
}

bool CoinManager::isValidCoinPosition(const glm::vec3& position, const std::vector<glm::vec3>& obstaclePositions,
                                    float obstacleRadius, float coinRadius) {
    for (const auto& obstaclePos : obstaclePositions) {
        float distance = glm::length(position - obstaclePos);
        if (distance < (obstacleRadius + coinRadius + 0.05f)) { 
            return false;
        }
    }
    return true;
}