// TODO:
// flashlight effect od gracza w kolorze niebieskim
// tekstura wulkanu, ścian i trawy

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>

#include "Camera.hpp"
#include "Player.hpp"
#include "InputHandler.hpp"
#include "MeshGenerator.hpp"
#include "Renderer.hpp"
#include "BubbleManager.hpp"
#include "ObstacleManager.hpp"
#include "Collision.hpp"
#include "MiniMap.hpp"
#include "UI.hpp"
#include "shader_utils.h"

GLFWwindow* window = nullptr;
Camera camera;
Player player;
InputHandler inputHandler;
Renderer renderer;
BubbleManager bubbleManager;
ObstacleManager obstacleManager;
MiniMap miniMap;
UI ui;
Mesh terrainMesh;
std::unique_ptr<Mesh> groundMesh;
std::unique_ptr<Mesh> boundaryWalls;

enum class AnimationState {
    NONE,
    LEVEL_COMPLETE,
    GAME_COMPLETE,
    PLAYER_DIED
};

AnimationState animationState = AnimationState::NONE;
float animationStartTime = 0.0f;
const float ANIMATION_DURATION = 3.0f;

int currentLevel = 1;
float gameTime = 0.0f;
bool gameRunning = true;
bool gameOver = false;

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void updateGame(float dt);
void checkCollisions();
void initializeGame();
void startAnimation(AnimationState state);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Helicopter Volcano Adventure", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (!renderer.initialize(SCR_WIDTH, SCR_HEIGHT)) {
        std::cout << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    initializeGame();
    ui.setWindowSize(SCR_WIDTH, SCR_HEIGHT);

    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (!glfwWindowShouldClose(window) && gameRunning) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        if (dt > 0.1f) dt = 0.1f;
        
        gameTime += dt;

        inputHandler.update(window);
        processInput(window);

        if (animationState == AnimationState::NONE) {
            inputHandler.update(window);
            processInput(window);
        } else {
            inputHandler.update(window);
            if (inputHandler.keys[GLFW_KEY_ESCAPE]) {
                gameRunning = false;
            }
        }

        if (!gameOver && animationState == AnimationState::NONE) {
            updateGame(dt);
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        renderer.render(camera, player, bubbleManager, terrainMesh, *groundMesh, *boundaryWalls, miniMap, gameTime, currentLevel);

        if (animationState != AnimationState::NONE) {
            float animationProgress = (gameTime - animationStartTime) / ANIMATION_DURATION;
            if (animationProgress > 1.0f) animationProgress = 1.0f;
            
            glm::vec3 animationColor;
            switch (animationState) {
                case AnimationState::LEVEL_COMPLETE:
                    animationColor = glm::vec3(1.0f, 1.0f, 1.0f); 
                    break;
                case AnimationState::GAME_COMPLETE:
                    animationColor = glm::vec3(0.0f, 1.0f, 0.0f); 
                    break;
                case AnimationState::PLAYER_DIED:
                    animationColor = glm::vec3(1.0f, 0.0f, 0.0f); 
                    break;
                default:
                    animationColor = glm::vec3(1.0f);
            }
            
            renderer.renderAnimation(animationProgress, animationColor);
            
            if (animationProgress >= 1.0f) {
                switch (animationState) {
                    case AnimationState::LEVEL_COMPLETE:
                        animationState = AnimationState::NONE;
                        break;
                    case AnimationState::GAME_COMPLETE:
                    case AnimationState::PLAYER_DIED:
                        gameRunning = false;
                        break;
                    default:
                        break;
                }
            }
        }

        if (gameOver) {
            ui.renderGameOver(currentLevel, gameTime);
        } else {
            ui.render(currentLevel, gameTime, player.lives, player.position);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    renderer.cleanup();
    terrainMesh.cleanup();
    glfwTerminate();

    if (gameOver) {
        std::cout << "Game Over!" << std::endl;
        std::cout << "Level reached: " << currentLevel << std::endl;
        std::cout << "Time survived: " << gameTime << " seconds" << std::endl;
        
    }

    return 0;
}

void startAnimation(AnimationState state) {
    animationState = state;
    animationStartTime = gameTime;
}

void initializeGame() {
    player = Player();
    player.position = glm::vec3(-4500.0f, 100.0f, -4500.0f);
    
    camera = Camera(glm::vec3(-4500.0f, 110.0f, -4490.0f));
    camera.setMode(THIRD_PERSON);
    
    terrainMesh = MeshGenerator::createVolcano(2000.0f, 500.0f, 3000.0f);
    terrainMesh.setupMesh();

    groundMesh = std::make_unique<Mesh>(std::move(MeshGenerator::createGroundPlane(10000.0f, 100)));
    groundMesh->setupMesh();

    boundaryWalls = std::make_unique<Mesh>(std::move(MeshGenerator::createBoundaryWalls(10000.0f, 10000.0f)));
    boundaryWalls->setupMesh();
    
    bubbleManager = BubbleManager();
    bubbleManager.setLevel(currentLevel);
    
    obstacleManager = ObstacleManager();
    obstacleManager.initializeVolcanoObstacles(terrainMesh);
    
    miniMap = MiniMap();
    miniMap.setWindowSize(SCR_WIDTH, SCR_HEIGHT);
}

void updateGame(float dt) {
    int forward = inputHandler.forward() - inputHandler.backward();
    int right = inputHandler.right() - inputHandler.left();
    int vertical = inputHandler.rise() - inputHandler.descend();
    
    player.processMovement(forward, right, vertical, inputHandler.sprint(), false);
    player.update(dt);
    
    switch (camera.currentMode) {
        case THIRD_PERSON:
            camera.updateThirdPerson(player.position, player.front, player.up);
            break;
        case FIRST_PERSON:
            camera.updateFirstPerson(player.position, player.front, player.up);
            break;
    }
    
    bool killedByBurst = bubbleManager.update(dt, player.position, currentLevel);
    if (killedByBurst) {
        player.lives = 0;
        // gameOver = true;
        startAnimation(AnimationState::PLAYER_DIED);
        std::cout << "Player killed by bubble burst!" << std::endl;
        return; 
    }

    checkCollisions();

    glm::vec3 craterCenter(0.0f, 3000.0f, 0.0f);
    float horizontalDistance = glm::length(glm::vec2(player.position.x, player.position.z) - glm::vec2(craterCenter.x, craterCenter.z));
    float verticalDistance = std::abs(player.position.y - craterCenter.y);

    // Check win condition (reached crater)
    float distanceToCrater = glm::length(player.position - glm::vec3(0.0f, 3000.0f, 0.0f));
    if (horizontalDistance < 200.0f && player.position.y > 2900.0f && player.position.y < 3100.0f) {
        if (currentLevel < 5) {
            // Level complete - start white animation
            startAnimation(AnimationState::LEVEL_COMPLETE);
            
            currentLevel++;
            bubbleManager.setLevel(currentLevel);
            std::cout << "Level up! Now at level " << currentLevel << std::endl;
            
            player.position = glm::vec3(-4500.0f, 100.0f, -4500.0f);
        }
        else {
            startAnimation(AnimationState::GAME_COMPLETE);
            std::cout << "Good job, you've won the game!" << std::endl;
        }
    }

    glm::vec3 volcanoPos(0.0f, 0.0f, 0.0f); 
    miniMap.update(player.position, volcanoPos);
}


void checkCollisions() {
    // Player vs terrain collision
    glm::vec3 closestPoint = CollisionSystem::closestPointOnMesh(
        player.position, terrainMesh.vertices, terrainMesh.indices);
    
    float terrainDistance = glm::length(player.position - closestPoint);
    if (terrainDistance < 5.0f) { 
        glm::vec3 normal = glm::normalize(player.position - closestPoint);
        player.resolveCollision(closestPoint, normal);
    }

    // Player vs boundary walls collision 
    const float boundary = 5000.0f;
    const float wallBuffer = 10.0f;
    
    if (player.position.x < -boundary + wallBuffer) {
        player.position.x = -boundary + wallBuffer;
        player.velocity.x = 0.0f;
    }
    if (player.position.x > boundary - wallBuffer) {
        player.position.x = boundary - wallBuffer;
        player.velocity.x = 0.0f;
    }
    if (player.position.z < -boundary + wallBuffer) {
        player.position.z = -boundary + wallBuffer;
        player.velocity.z = 0.0f;
    }
    if (player.position.z > boundary - wallBuffer) {
        player.position.z = boundary - wallBuffer;
        player.velocity.z = 0.0f;
    }

    // Player vs bubble collisions
    auto aliveBubbles = bubbleManager.getAliveBubbles();
    for (const auto& bubble : aliveBubbles) {
        auto spherePositions = player.getCollisionSpherePositions();
        for (const auto& spherePos : spherePositions) {
            if (CollisionSystem::sphereSphereCollision(
                spherePos, 2.0f, bubble.position, bubble.currentRadius)) {
                player.lives = 0;
                
                if (player.lives <= 0) {
                    startAnimation(AnimationState::PLAYER_DIED);
                    std::cout << "Player died from bubble collision!" << std::endl;
                }
                return; 
            }
        }
    }
    
}

void processInput(GLFWwindow* window) {
    // Toggle transparency with T key 
    static bool tKeyPressedLastFrame = false;
    bool tKeyPressedThisFrame = inputHandler.keys[GLFW_KEY_T];
    
    if (tKeyPressedThisFrame && !tKeyPressedLastFrame) {
        renderer.useTransparency = !renderer.useTransparency;
        std::cout << "Transparency: " << (renderer.useTransparency ? "ON" : "OFF") << std::endl;
    }
    tKeyPressedLastFrame = tKeyPressedThisFrame;
    
    // Switch camera mode with TAB key
    static bool tabPressedLastFrame = false;
    bool tabPressedThisFrame = inputHandler.keys[GLFW_KEY_TAB];
    
    if (tabPressedThisFrame && !tabPressedLastFrame) {
        if (camera.currentMode == THIRD_PERSON) {
            camera.setMode(FIRST_PERSON);
            std::cout << "Switched to FIRST PERSON view" << std::endl;
        } else {
            camera.setMode(THIRD_PERSON);
            std::cout << "Switched to THIRD PERSON view" << std::endl;
        }
    }
    tabPressedLastFrame = tabPressedThisFrame;
    
    if (inputHandler.keys[GLFW_KEY_ESCAPE]) {
        gameRunning = false;
    }
    
    if (animationState == AnimationState::NONE) {
        player.processMouseMovement(inputHandler.mouseDelta.x, inputHandler.mouseDelta.y);
        
        camera.processMouseScroll(inputHandler.mouseScroll);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    const float targetAspect = 3.0f / 2.0f;
    
    float currentAspect = (float)width / (float)height;
    
    int viewportWidth, viewportHeight, viewportX, viewportY;
    
    if (currentAspect > targetAspect) {
        viewportHeight = height;
        viewportWidth = (int)(height * targetAspect);
        viewportX = (width - viewportWidth) / 2;
        viewportY = 0;
    } else {
        viewportWidth = width;
        viewportHeight = (int)(width / targetAspect);
        viewportX = 0;
        viewportY = (height - viewportHeight) / 2;
    }
    
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
    
    renderer.setWindowSize(viewportWidth, viewportHeight);
    ui.setWindowSize(width, height); 
    miniMap.setWindowSize(viewportWidth, viewportHeight);  
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    inputHandler.mousePosition = glm::vec2(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    inputHandler.mouseScroll = yoffset;
    camera.processMouseScroll(yoffset);
}