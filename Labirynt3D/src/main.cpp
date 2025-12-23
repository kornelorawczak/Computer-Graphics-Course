#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

#include "Camera.hpp"
#include "Player.hpp"
#include "ObstacleManager.hpp"
#include "Renderer.hpp"
#include "InputHandler.hpp"
#include "Collision.hpp"
#include "MiniMap.hpp"
#include "CoinManager.hpp"
#include "shader_utils.h"


// Global variables
GLFWwindow* window = nullptr;
Camera camera;
Player player;
ObstacleManager obstacleManager;
Renderer renderer;
InputHandler inputHandler;
MiniMap miniMap;
CoinManager coinManager;
int score = 0;

// Window dimensions
int windowWidth = 1200;
int windowHeight = 1200;

// Time tracking
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float gameStartTime = 0.0f;
float gameEndTime = 0.0f;
bool gameWon = false;
float winAnimationProgress = 0.0f;

//win Animation
GLuint winShader;
GLuint winVAO, winVBO;
void setupWinAnimation();
void renderWinAnimation(float progress);
void cleanupWinAnimation();

// Function prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void updateViewport();
bool initializeGLFW();
bool initializeGLAD();
void printHelp();

int main(int argc, char* argv[]) {
    int gridSize = 5;
    int seed = 0;
    
    if (argc >= 2) {
        seed = std::atoi(argv[1]);
    } else {
        seed = static_cast<int>(std::time(nullptr));
    }
    
    if (argc >= 3) {
        gridSize = std::atoi(argv[2]);
    }
    
    printHelp();
    
    if (!initializeGLFW()) {
        return -1;
    }
    
    if (!initializeGLAD()) {
        glfwTerminate();
        return -1;
    }
    
    if (!renderer.initialize()) {
        std::cout << "Failed to initialize renderer!" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    obstacleManager.generateObstacles(gridSize, seed);
    setupWinAnimation();
    
    std::vector<glm::vec3> obstaclePositions;
    for (const auto& obstacle : obstacleManager.getObstacles()) {
        obstaclePositions.push_back(obstacle.position);
    }
    coinManager.generateCoins(10, obstaclePositions, 
                             obstacleManager.getObstacleRadius(), gridSize, seed + 123);

    // Set camera position to player position with offset
    camera.setPosition(player.getPosition() + player.getCameraOffset());
    
    gameStartTime = glfwGetTime();

    // Game loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        inputHandler.update(window);

        miniMap.update(deltaTime);
        
        obstacleManager.updateObstacles(deltaTime);

        coinManager.checkPlayerCollision(player.getPosition(), player.getRadius(), score);

        renderer.clear();
        
        updateViewport();

        // handle win
        if (gameWon) {
            winAnimationProgress += deltaTime / 3.0f;
            if (winAnimationProgress >= 1.0f) {
                float totalTime = gameEndTime - gameStartTime;
                std::cout << "Congratulations! You completed the maze in " << totalTime << " seconds!" << std::endl;
                glfwSetWindowShouldClose(window, true);
            }
            glm::mat4 view = camera.getViewMatrix();
            float aspectRatio = (windowWidth > windowHeight) ? 
            (float)windowHeight / (float)windowWidth : 1.0f;
            glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
            
            // Render for fps view
            renderer.renderCubeWalls(view, projection);
            renderer.renderObstacles(obstacleManager.getObstacles(), view, projection);
            renderWinAnimation(winAnimationProgress);
            
            renderer.renderCoins(coinManager.getCoins(), view, projection);
            
            // Render minimap 
            renderer.renderMiniMap(miniMap, obstacleManager.getObstacles(), 
                                coinManager.getCoins(),
                                player.getPosition(), player.getRadius());
        }
        else {
            // Get view and projection matrices for fps view
            glm::mat4 view = camera.getViewMatrix();
            float aspectRatio = (windowWidth > windowHeight) ? 
                            (float)windowHeight / (float)windowWidth : 1.0f;
            glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
            
            // Render for fps view
            renderer.renderCubeWalls(view, projection);
            renderer.renderObstacles(obstacleManager.getObstacles(), view, projection);
            
            renderer.renderCoins(coinManager.getCoins(), view, projection);
            
            // Render minimap 
            renderer.renderMiniMap(miniMap, obstacleManager.getObstacles(), 
                                coinManager.getCoins(),
                                player.getPosition(), player.getRadius());

            if (player.checkGoalReached() && !player.hasReachedGoal() && score==10) {
                player.setReachedGoal(true);
                gameWon = true;
                gameEndTime = glfwGetTime();
            }
        }

        // End input frame (update previous key states)
        inputHandler.endFrame();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    cleanupWinAnimation();
    renderer.cleanup();
    glfwTerminate();
    
    return 0;
}

// void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
//     windowWidth = width;
//     windowHeight = height;
//     // Update minimap viewport when window resizes
//     int minimapSize = std::min(windowWidth, windowHeight) / 4; 
//     int minimapX = windowWidth; 
//     int minimapY = windowHeight; 
//     miniMap.setViewport(minimapX, minimapY, minimapSize, minimapSize);
// }

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    inputHandler.processMouseMovement(xpos, ypos);
    
    if (inputHandler.isMouseCaptured()) {
        camera.processMouseMovement(inputHandler.getMouseXOffset(), inputHandler.getMouseYOffset());
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    inputHandler.processKey(key, action);
}

void processInput(GLFWwindow* window) {
    if (inputHandler.isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
    }

    if (inputHandler.isKeyJustPressed(GLFW_KEY_1)) {
        miniMap.switchViewSide(MiniMap::ViewSide::FRONT);
    }
    if (inputHandler.isKeyJustPressed(GLFW_KEY_2)) {
        miniMap.switchViewSide(MiniMap::ViewSide::SIDE);
    }
    
    
    // Player movement with collision detection
    glm::vec3 movement(0.0f);
    float moveSpeed = 1.0f * deltaTime;
    
    if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
        movement += camera.getFront() * moveSpeed;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
        movement -= camera.getFront() * moveSpeed;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
        movement -= glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * moveSpeed;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
        movement += glm::normalize(glm::cross(camera.getFront(), camera.getUp())) * moveSpeed;
    }
    
    glm::vec3 newPosition = player.getPosition() + movement;
    
    // Check world boundaries
    glm::vec3 clampedPosition = newPosition;
    Collision::checkWorldBoundaries(newPosition, player.getRadius(), clampedPosition);
    
    // Check collisions with obstacles
    bool collision = false;
    for (const auto& obstacle : obstacleManager.getObstacles()) {
        if (Collision::checkSphereObstacleCollision(clampedPosition, player.getRadius(), obstacle)) {
            collision = true;
            break;
        }
    }
    
    // Update player position if no collision
    if (!collision) {
        player.updatePosition(clampedPosition);
        // Update camera position to follow player
        camera.setPosition(player.getPosition() + player.getCameraOffset());
    }
}

void updateViewport() {
    // Calculate centered square viewport
    int size = std::min(windowWidth, windowHeight);
    int x = (windowWidth - size) / 2;
    int y = (windowHeight - size) / 2;
    renderer.setViewport(x, y, size, size);
}

bool initializeGLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(windowWidth, windowHeight, "3D Maze Game", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    //glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    
    return true;
}

bool initializeGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

void setupWinAnimation() {  
    winShader = loadShader("../shaders/win_vert.glsl", "../shaders/win_frag.glsl");
    if (!winShader) {
        std::cout << "Failed to load win shader" << std::endl;
        return;
    }
    // Fullscreen quad vertices
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &winVAO);
    glGenBuffers(1, &winVBO);
    
    glBindVertexArray(winVAO);
    glBindBuffer(GL_ARRAY_BUFFER, winVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void renderWinAnimation(float progress) {
    // Enable blending for the overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use win shader
    glUseProgram(winShader);
    
    // Set uniforms
    GLint progressLoc = glGetUniformLocation(winShader, "progress");
    GLint resolutionLoc = glGetUniformLocation(winShader, "resolution");
    glUniform1f(progressLoc, progress);
    glUniform2f(resolutionLoc, (float)windowWidth, (float)windowHeight);
    
    // Render fullscreen quad
    glBindVertexArray(winVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // Disable blending
    glDisable(GL_BLEND);
}

void cleanupWinAnimation() {
    if (winVAO) glDeleteVertexArrays(1, &winVAO);
    if (winVBO) glDeleteBuffers(1, &winVBO);
    if (winShader) glDeleteProgram(winShader);
}

void printHelp() {
    std::cout << "\nControls:" << std::endl;
    std::cout << "WASD - Move around" << std::endl;
    std::cout << "Mouse - Look around" << std::endl;
    std::cout << "ESC - Exit game" << std::endl;
    std::cout << "Goal: Reach the opposite corner (1,1,1) from start (0,0,0)" << std::endl;
    std::cout << "Obstacle colors indicate position: red=X, green=Y, blue=Z" << std::endl;
}