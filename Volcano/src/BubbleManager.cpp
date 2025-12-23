#include "BubbleManager.hpp"
#include <cmath>
#include <iostream>

Bubble::Bubble(const glm::vec3& pos, float radius, float growth, float speed, const glm::vec3& col)
    : position(pos), initialRadius(radius), currentRadius(radius), growthRate(growth), 
      riseSpeed(speed), color(col), alive(true), lifeTimer(0.0f) {
}

void Bubble::update(float dt) {
    if (!alive) return;
    
    lifeTimer += dt;
    
    position.y += riseSpeed * dt;
    
    currentRadius += growthRate * dt;
    
    position.x += (sin(lifeTimer * 2.0f) * 0.1f) * dt;
    position.z += (cos(lifeTimer * 2.0f) * 0.1f) * dt;
}

bool Bubble::shouldBurst() const {
    return currentRadius >= initialRadius * 2.0f;
}

float Bubble::getBurstRadius() const {
    return currentRadius * 10.0f; 
}

BubbleManager::BubbleManager() : spawnTimer(0.0f), currentLevel(1) {
    rng.seed(std::random_device()());
    initializeLevelParams();
}

bool BubbleManager::update(float dt, const glm::vec3& playerPos, int currentLevel) {
    this->currentLevel = currentLevel;
    bool playerKilled = false;
    
    for (auto& bubble : bubbles) {
        if (bubble.alive) {
            bubble.update(dt);
            
            if (bubble.shouldBurst()) {
                float burstRadius = bubble.getBurstRadius();
                float distanceToPlayer = glm::length(playerPos - bubble.position);
                
                if (distanceToPlayer < burstRadius) {
                    std::cout << "BUBBLE BURST! Player within burst radius of " << burstRadius 
                              << "m. Distance: " << distanceToPlayer << "m" << std::endl;
                    playerKilled = true;
                }
                bubble.alive = false;
            }
        }
    }
    
    bubbles.erase(std::remove_if(bubbles.begin(), bubbles.end(),
        [](const Bubble& b) { return !b.alive; }), bubbles.end());
    
    spawnTimer += dt;
    LevelParams params = levelParams[currentLevel - 1];
    float spawnInterval = 1.0f / params.spawnRate;
    
    while (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;
        spawnRandomBubble(100.0f); 
    }

    return playerKilled;
}

bool BubbleManager::checkBurstDamage(const glm::vec3& playerPos) {
    for (auto& bubble : bubbles) {
        if (bubble.alive && bubble.shouldBurst()) {
            float distanceToPlayer = glm::length(playerPos - bubble.position);
            if (distanceToPlayer < bubble.getBurstRadius()) {
                return true;
            }
        }
    }
    return false;
}

void BubbleManager::spawnBubble(const glm::vec3& position) {
    LevelParams params = levelParams[currentLevel - 1];
    
    std::uniform_real_distribution<float> radiusDist(params.minRadius, params.maxRadius);
    std::uniform_real_distribution<float> growthDist(params.minGrowth, params.maxGrowth);
    std::uniform_real_distribution<float> speedDist(params.minSpeed, params.maxSpeed);
    std::uniform_real_distribution<float> colorDist(0.2f, 0.9f);
    
    float radius = radiusDist(rng);
    float growth = growthDist(rng);
    float speed = speedDist(rng);
    glm::vec3 color(colorDist(rng), colorDist(rng), colorDist(rng));
    
    bubbles.emplace_back(position, radius, growth, speed, color);
}

void BubbleManager::spawnRandomBubble(float terrainHeight) {
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> radiusDist(0.0f, 4500.0f); 
    std::uniform_real_distribution<float> heightDist(50.0f, 4000.0f); 

    float angle = angleDist(rng);
    float radius = radiusDist(rng);
    float height = heightDist(rng); 
    
    glm::vec3 position;
    position.x = cos(angle) * radius;
    position.y = height; 
    position.z = sin(angle) * radius;
    
    spawnBubble(position);
}

std::vector<Bubble> BubbleManager::getAliveBubbles() const {
    std::vector<Bubble> aliveBubbles;
    for (const auto& bubble : bubbles) {
        if (bubble.alive) {
            aliveBubbles.push_back(bubble);
        }
    }
    return aliveBubbles;
}

int BubbleManager::getSpawnRateForLevel(int level) const {
    if (level < 1 || level > static_cast<int>(levelParams.size())) {
        return levelParams.back().spawnRate;
    }
    return levelParams[level - 1].spawnRate;
}

void BubbleManager::initializeLevelParams() {
    levelParams = {
        // Level 1
        {30, 2.0f, 10.0f, 0.2f, 0.45f, 7.0f, 20.0f},
        // Level 2
        {50, 3.0f, 15.0f, 0.3f, 0.55f, 8.5f, 30.0f},
        // Level 3
        {70, 4.0f, 20.0f, 0.45f, 0.7f, 10.0f, 40.0f},
        // Level 4
        {90, 5.0f, 25.0f, 0.6f, 0.85f, 12.0f, 50.0f},
        // Level 5
        {120, 6.0f, 35.0f, 0.7f, 0.95f, 15.0f, 60.0f}
    };
}