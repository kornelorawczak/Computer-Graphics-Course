#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "CoinManager.hpp"

struct Mesh;
struct Obstacle;
class MiniMap;

class Renderer {
    public:
    Renderer();
    ~Renderer();
    
    bool initialize();
    void cleanup();
    
    void setViewport(int x, int y, int width, int height);
    void clear();
    
    void renderObstacles(const std::vector<Obstacle>& obstacles, 
                         const glm::mat4& view, const glm::mat4& projection);
    void renderCubeWalls(const glm::mat4& view, const glm::mat4& projection);
    void renderPlayerSphere(const glm::vec3& position, float radius,
                            const glm::mat4& view, const glm::mat4& projection);
    void renderMiniMap(const MiniMap& miniMap,
                       const std::vector<Obstacle>& obstacles,
                       const std::vector<Coin>& coins,
                       const glm::vec3& playerPosition, float playerRadius);
    void renderCubeWireframe(const glm::mat4& view, const glm::mat4& projection);    
    void renderCoins(const std::vector<Coin>& coins, const glm::mat4& view, const glm::mat4& projection);


    void setupObstacleMesh();
    void setupCubeMesh();
    void setupSphereMesh();
    void setupWireframeCubeMesh();
    void setupCoinMesh();
    
    
private:
    GLuint obstacleShader;
    GLuint wallShader;
    GLuint sphereShader;
    GLuint wallTexture;   
    GLuint wireframeShader;
    GLuint coinShader;
    
    
    GLuint obstacleVAO, obstacleVBO, obstacleEBO;
    GLuint cubeVAO, cubeVBO, cubeEBO;
    GLuint sphereVAO, sphereVBO, sphereEBO;
    GLuint wireframeVAO, wireframeVBO, wireframeEBO;
    GLuint coinVAO, coinVBO, coinEBO;
    
    unsigned int obstacleIndexCount;
    unsigned int cubeIndexCount;
    unsigned int sphereIndexCount;
    unsigned int wireframeIndexCount;
    unsigned int coinIndexCount;
    
    int viewportX, viewportY, viewportWidth, viewportHeight;
    
    GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);
    void setupMeshBuffers(const Mesh& mesh, GLuint& VAO, GLuint& VBO, GLuint& EBO);
};

#endif