#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "Camera.hpp"
#include "Player.hpp"
#include "BubbleManager.hpp"

class Camera;
class Player;
class BubbleManager;
class MiniMap;
struct Mesh;

class Renderer {
public:
    Renderer();
    ~Renderer();
    
    bool initialize(int width, int height);
    void cleanup();
    
    void render(const Camera& camera, const Player& player, const BubbleManager& bubbleManager, 
                const Mesh& terrain, const Mesh& ground, const Mesh& walls, 
                const MiniMap& miniMap, float gameTime, int currentLevel);
    
    void setWindowSize(int width, int height);
    void renderAnimation(float progress, const glm::vec3& color);
    void Renderer::setupFlashlightUniforms(GLuint shader, const Camera& camera, const Player& player);
    
    bool useTransparency;
    bool useFog;
    bool useFlashlight;
    float fogDensity;
    glm::vec3 fogColor;
    
private:
    void loadShaders();
    
    void Renderer::renderOpaque(const Camera& camera, const Player& player, const Mesh& terrain, const Mesh& ground, const Mesh& walls);
    void renderTransparentBubbles(const Camera& camera, const Player& player, const BubbleManager& bubbleManager);
    void renderPlayer(const Camera& camera, const Player& player);
    void renderUI(int currentLevel, float gameTime);
    void renderMiniMap(const MiniMap& miniMap, const Player& player, const Mesh& terrain);
    void renderHelicopterBlades(const Camera& camera, const Player& player);
    void setupAnimation();
    void cleanupAnimation();
    
    unsigned int phongShader;
    unsigned int instancedSphereShader;
    unsigned int minimapShader;
    unsigned int uiShader;
    
    struct InstanceData {
        glm::vec3 position;
        float scale;
        glm::vec3 color;
        float alpha;
    };
    
    std::vector<InstanceData> bubbleInstanceData;
    unsigned int instanceVBO;
    
    int windowWidth, windowHeight;
    
    Mesh* sphereMesh;
    Mesh* bladeMesh;
    GLuint animationShader;
    GLuint animationVAO, animationVBO;
    Mesh* animationQuad;
    Mesh* arrowMesh;
    
    // Frame timing
    double lastTime;
    int frameCount;
    float fps;
};

#endif