#ifndef BUBBLEMANAGER_HPP
#define BUBBLEMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <random>

struct Bubble {
    glm::vec3 position;
    float initialRadius;
    float currentRadius;
    float growthRate;
    float riseSpeed;
    glm::vec3 color;
    bool alive;
    float lifeTimer;
    
    Bubble(const glm::vec3& pos, float radius, float growth, float speed, const glm::vec3& col);
    void update(float dt);
    bool shouldBurst() const;
    float getBurstRadius() const;
};

class BubbleManager {
public:
    BubbleManager();
    
    bool update(float dt, const glm::vec3& playerPos, int currentLevel);
    bool checkBurstDamage(const glm::vec3& playerPos);
    void spawnBubble(const glm::vec3& position);
    void spawnRandomBubble(float terrainHeight);
    
    const std::vector<Bubble>& getBubbles() const { return bubbles; }
    std::vector<Bubble> getAliveBubbles() const;
    
    
    void setLevel(int level) { currentLevel = level; }
    int getSpawnRateForLevel(int level) const;

    
private:
    std::vector<Bubble> bubbles;
    std::default_random_engine rng;
    
    float spawnTimer;
    int currentLevel;
    
    struct LevelParams {
        int spawnRate;      
        float minRadius;
        float maxRadius;
        float minGrowth;
        float maxGrowth;
        float minSpeed;
        float maxSpeed;
    };
    
    std::vector<LevelParams> levelParams;
    void initializeLevelParams();
};

#endif