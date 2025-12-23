#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "shader_utils.h"
#include <glm/glm.hpp>
#include <iomanip>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <string>

using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    int size = (width < height) ? width : height; // rendered window size will be the largest possible square 
    int xOffset = (width - size) / 2;
    int yOffset = (height - size) / 2;
    glViewport(xOffset, yOffset, size, size);
}

float crossProduct(vec2 a, vec2 b) {
    return a.x * b.y - a.y * b.x;
}

//https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
bool segmentsIntersect(vec2 p1, vec2 p2, vec2 q1, vec2 q2) {
    vec2 v1 = p2 - p1; // vector of a segment p1-p2
    vec2 v2 = q2 - q1; // vector of a segment q1-q2
    float v1Xv2 = crossProduct(v1, v2);
    vec2 pq = q1 - p1; 
    float pqXv1 = crossProduct(pq, v1);

    if (v1Xv2 == 0) return false; // parallel 

    float t = crossProduct(pq, v2) / v1Xv2;
    float u = crossProduct(pq, v1) / v1Xv2;

    return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Triangle", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    float vertices[] = {
        // positions        // colors (R, G, B)
        -0.9f, -0.9f, 0.0f,   0.0f, 0.0f, 0.0f,   // lewy dół  (czarny)
        -0.9f,  0.9f, 0.0f,   0.0f, 0.0f, 1.0f,   // lewy góra (niebieski)
        0.9f,  0.9f, 0.0f,   0.0f, 1.0f, 0.0f,   // prawy góra (zielony)

        -0.9f, -0.9f, 0.0f,   0.0f, 0.0f, 0.0f,   // lewy dół  (czarny)
        0.9f,  0.9f, 0.0f,   0.0f, 1.0f, 0.0f,   // prawy góra (zielony)
        0.9f, -0.9f, 0.0f,   1.0f, 0.0f, 0.0f    // prawy dół  (czerwony)
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // binding the aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // binding the aColor - 3 floats but with offset of 3 because first is position of vertex
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int shaderProgram = createShaderProgram("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    // flagged variables (./buildRun.bat 15 123 means GRID_N = 15 and seed = 123)
    int GRID_N = 10;
    unsigned int customSeed = 0;
    if (argc >= 2) { 
        GRID_N = std::atoi(argv[1]);
        if (GRID_N <= 0) GRID_N = 10;
    }
    if (argc >= 3) {
        customSeed = static_cast<unsigned int>(std::atoi(argv[2]));
    }
    
    
    // Rhombus generator
    const float left = -0.9f;
    const float right = 0.9f;
    const float bottom = -0.9f;
    const float top = -0.9f;
    const float step = (right - left) / (GRID_N - 1);
    
    const float long_diag = 1.8f / GRID_N;
    const float short_diag = 0.5f / GRID_N;
    
    if (customSeed != 0) std::srand(customSeed);
    else std::srand(static_cast<unsigned int>(std::time(nullptr))); // seed (zmień na stały numer, jeśli chcesz powtarzalność)
    const float PI = 3.14159265358979323846f;
    
    float local[4][2] = {
        { 0.0f, long_diag / 2.0f  },
        { short_diag / 2.0f, 0.0f },
        { 0.0f, -long_diag / 2.0f },
        { -short_diag / 2.0f, 0.0f}
    };

    std::vector<float> rhombusVertices;
    std::vector<std::array<vec2, 4>> obstacles;
    
    for (int j = 0; j < GRID_N; j++) {
        for (int i = 0; i < GRID_N; i++) {
            if ((i == 0 && j == 0) || (i == GRID_N - 1 && j == GRID_N - 1)) continue;
            
            float center_x = left + i * step;
            float center_y = bottom + j * step;
            
            float rotation = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX); // between 0 and 1
            float angle = rotation * 2.0f * PI;
            float cos = std::cos(angle);
            float sin = std::sin(angle);
            
            auto pushVertex = [&](float lx, float ly) {
                float rx = lx * cos - ly * sin;
                float ry = lx * sin + ly * cos;
                rhombusVertices.push_back(center_x + rx); 
                rhombusVertices.push_back(center_y + ry); 
                rhombusVertices.push_back(0.0f); 
            };
            // triangle 1 to form a rhombus
            pushVertex(local[0][0], local[0][1]);
            pushVertex(local[1][0], local[1][1]);
            pushVertex(local[2][0], local[2][1]);
            
            // triangle 2 to form a rhombus
            pushVertex(local[2][0], local[2][1]);
            pushVertex(local[3][0], local[3][1]);
            pushVertex(local[0][0], local[0][1]);

            std::array<vec2, 4> verts;
            for (int v = 0; v < 4; v++) {
                float lx = local[v][0], ly = local[v][1];
                float rx = lx * cos - ly * sin;
                float ry = lx * sin + ly * cos;
                verts[v] = vec2(center_x + rx, center_y + ry);
            }
            obstacles.push_back(verts);
        }
    }
    // Collision between two polygons
    auto dot = [&](const std::pair<float, float>& p1, const std::pair<float, float>& p2) -> float {
        return p1.first * p2.first + p1.second * p2.second;
    };


    // create Rhombus VAO and VBO
    unsigned int rhombusVAO, rhombusVBO;
    glGenVertexArrays(1, &rhombusVAO);
    glGenBuffers(1, &rhombusVBO);
    
    glBindVertexArray(rhombusVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rhombusVBO);
    glBufferData(GL_ARRAY_BUFFER, rhombusVertices.size() * sizeof(float), rhombusVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // load rhombus shader 
    unsigned int rhombusShader = createShaderProgram("../shaders/rhombus_vertex.glsl", "../shaders/rhombus_fragment.glsl");
    
    // Player rhombus
    std::vector<float> playerRhombus(6 * 6);
    float playerAngle = -45.0f * PI / 180.0f;
    float playerCos = std::cos(playerAngle);
    float playerSin = std::sin(playerAngle);
    float player_center_x = -0.9f;
    float player_center_y = -0.9f;
    float moveSpeed = 0.02f;
    float rotateSpeed = PI / 120;

    // Finish line Triangle
    float finishVertices[] = {
        0.080f,  0.050f, 0.0f,
        -0.08f,  0.050f, 0.0f,
        0.000f, -0.085f, 0.0f
    };
    float finishLocal[3][2] = {
        {0.08f, 0.05f},
        {-0.08f, 0.05f},
        {0.0f, -0.085f}
    };
    vec2 finishCenter = vec2(0.9f, 0.9f);
    const float finishRotationSpeed = 0.45f;
    bool finishVisible = true;

    auto calculateBrightness = [&](float pX, float pY) -> float {
        vec2 playerPos = vec2(pX, pY);

        float distance = length(finishCenter - playerPos);
        float maxDistance = length(vec2(1.8f, 1.8f));
        float normalizedDistance = 1.0f - (distance / maxDistance);
        float brightness = 0.45f + 0.8f * normalizedDistance;
        
        return clamp(brightness, 0.45f, 1.0f);
};

    auto updatePlayerVertices = [&]() {
        float cosA = std::cos(playerAngle);
        float sinA = std::sin(playerAngle);
        float brightness = calculateBrightness(player_center_x, player_center_y);

        auto addPlayerVertex = [&](int i, float r, float g, float b, int vertIndex) {
            float x = local[i][0] * cosA - local[i][1] * sinA + player_center_x;
            float y = local[i][0] * sinA + local[i][1] * cosA + player_center_y;
            playerRhombus[vertIndex*6 + 0] = x;
            playerRhombus[vertIndex*6 + 1] = y;
            playerRhombus[vertIndex*6 + 2] = 0.0f;

            playerRhombus[vertIndex*6 + 3] = r * brightness;
            playerRhombus[vertIndex*6 + 4] = g * brightness;
            playerRhombus[vertIndex*6 + 5] = b * brightness;
        };

        // triangle 1: top, right, bottom
        addPlayerVertex(0, 1.0f, 0.84f, 0.0f, 0); // top (white)
        addPlayerVertex(1, 1.0f, 0.84f, 0.0f, 1); // right (white)
        addPlayerVertex(2, 0.29f, 0.96f, 1.0f, 2); // bottom (white)

        // triangle 2: bottom, left, right
        addPlayerVertex(2, 0.29f, 0.96f, 1.0f, 3); // bottom (white)
        addPlayerVertex(3, 1.0f, 0.84f, 0.0f, 4); // left (white)
        addPlayerVertex(0, 1.0f, 0.84f, 0.0f, 5); // right (gold)
    };

    updatePlayerVertices();
    unsigned int playerVAO, playerVBO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);
    glBindVertexArray(playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, playerRhombus.size() * sizeof(float), playerRhombus.data(), GL_DYNAMIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int playerShaderProgram = createShaderProgram("../shaders/player_vertex.glsl", "../shaders/player_fragment.glsl");

    bool gameWon = false;
    double winTime = 0.0;
    double gameStartTime = glfwGetTime();
    bool animationStarted = false;
    double animationStartTime = 0.0;

    unsigned int postProcessShader = createShaderProgram("../shaders/postprocess_vertex.glsl", "../shaders/postprocess_fragment.glsl");

    unsigned int finishVAO, finishVBO;
    glGenVertexArrays(1, &finishVAO);
    glGenBuffers(1, &finishVBO);

    glBindVertexArray(finishVAO);
    glBindBuffer(GL_ARRAY_BUFFER, finishVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(finishVertices), finishVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int finishShaderProgram = createShaderProgram("../shaders/finish_vertex.glsl", "../shaders/finish_fragment.glsl");

    // Check player collsion after the desired move or rotation as a possible intersection of any 4 segments of player with any 
    // 4 segments of any obstacle
    auto PlayerWillCollide = [&](float nextX, float nextY, float nextAngle) {
        std::array<vec2, 4> playerVerts;
        float cos = std::cos(nextAngle);
        float sin = std::sin(nextAngle);
        for (int i = 0; i < 4; i++) {
            float x = local[i][0] * cos - local[i][1] * sin + nextX;
            float y = local[i][0] * sin + local[i][1] * cos + nextY;
            playerVerts[i] = vec2(x, y);
        }

        for (const auto& obs : obstacles) {
            for (int i = 0; i < 4; i++) {
                vec2 p1 = playerVerts[i];
                vec2 p2 = playerVerts[(i + 1) % 4];
                for (int j = 0; j < 4; j++) {
                    vec2 q1 = obs[j];
                    vec2 q2 = obs[(j + 1) % 4];
                    if (segmentsIntersect(p1, p2, q1, q2)) return true;
                }
            }
        }
        return false;
    };

    auto PlayerFinishCollision = [&](float currentX, float currentY, float currentAngle, float timeValue) {
        if (!finishVisible) return false;

        std::array<vec2, 4> playerVerts;
        float cos = std::cos(currentAngle);
        float sin = std::sin(currentAngle);
        for (int i = 0; i < 4; i++) {
            float x = local[i][0] * cos - local[i][1] * sin + currentX;
            float y = local[i][0] * sin + local[i][1] * cos + currentY;
            playerVerts[i] = vec2(x, y);
        }

        float finishAngle = timeValue * finishRotationSpeed;
        float fcos = std::cos(finishAngle);
        float fsin = std::sin(finishAngle);
        std::array<vec2, 3> finishVertsWorld;
        for (int i = 0; i < 3; i++) {
            float lx = finishLocal[i][0];
            float ly = finishLocal[i][1];
            float rx = lx * fcos - ly * fsin;
            float ry = lx * fsin + ly * fcos;
            finishVertsWorld[i] = vec2(finishCenter.x + rx, finishCenter.y + ry);
        }

        for (int i = 0; i < 4; i++) {
            vec2 p1 = playerVerts[i];
            vec2 p2 = playerVerts[(i + 1) % 4];
            for (int j = 0; j < 3; j++) {
                vec2 q1 = finishVertsWorld[j];
                vec2 q2 = finishVertsWorld[(j + 1) % 3];
                if (segmentsIntersect(p1, p2, q1, q2)) return true;
            }
        }

        return false;
    };

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Framebuffer for post processing
    unsigned int frameBuffer, textureColorBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glGenTextures(1, &textureColorBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Fullscreen quad for post-processing
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- Pętla renderująca ---
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        float timeValue = static_cast<float>(glfwGetTime());
        int timeLoc = glGetUniformLocation(shaderProgram, "time");
        glUniform1f(timeLoc, timeValue);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUseProgram(rhombusShader);
        glBindVertexArray(rhombusVAO);
        glDrawArrays(GL_TRIANGLES, 0, (GRID_N * GRID_N - 2) * 6);

        glUseProgram(finishShaderProgram);
        int finishTimeLoc = glGetUniformLocation(finishShaderProgram, "time");
        glUniform1f(finishTimeLoc, timeValue);
        if (finishVisible) {
            glBindVertexArray(finishVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glUseProgram(playerShaderProgram);
        glBindVertexArray(playerVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        float moveStep = moveSpeed * deltaTime;
        float rotateStep = rotateSpeed * deltaTime;

        // Press ESC to exit game
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        
        // Press M to win game - debug
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
            gameWon = true;
        }


        // Player Input only when game is in progress
        if (!gameWon) {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                float dx = (local[0][0] * std::cos(playerAngle) - local[0][1] * std::sin(playerAngle)) * moveSpeed;
                float dy = (local[0][0] * std::sin(playerAngle) + local[0][1] * std::cos(playerAngle)) * moveSpeed;
                if (!PlayerWillCollide(player_center_x + dx, player_center_y + dy, playerAngle)) { 
                    player_center_x += dx;
                    player_center_y += dy;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                float dx = (local[2][0] * std::cos(playerAngle) - local[2][1] * std::sin(playerAngle)) * moveSpeed;
                float dy = (local[2][0] * std::sin(playerAngle) + local[2][1] * std::cos(playerAngle)) * moveSpeed;
                if (!PlayerWillCollide(player_center_x + dx, player_center_y + dy, playerAngle)) { 
                    player_center_x += dx;
                    player_center_y += dy;
                }
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                float nextAngle = playerAngle + rotateSpeed;
                if (!PlayerWillCollide(player_center_x, player_center_y, nextAngle)) { 
                    playerAngle = nextAngle;
                }
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                float nextAngle = playerAngle - rotateSpeed;
                if (!PlayerWillCollide(player_center_x, player_center_y, nextAngle)) { 
                    playerAngle = nextAngle;
                }
            }
        }

        updatePlayerVertices(); // recalc vertices based on new center + angle
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, playerRhombus.size() * sizeof(float), playerRhombus.data());

        if (finishVisible && !gameWon) {
            if (PlayerFinishCollision(player_center_x, player_center_y, playerAngle, timeValue)) {
                finishVisible = false;
                gameWon = true;
                winTime = glfwGetTime();
            }
        }

        if (gameWon && !animationStarted && (currentFrame - winTime >= 3.0)) {
            animationStarted = true;
            animationStartTime = currentFrame;
        }

        if (gameWon && animationStarted && (currentFrame - winTime >= 5.0)) {
            glfwSetWindowShouldClose(window, true);
            double totalSeconds = winTime - gameStartTime;
            std::cout << std::fixed << std::setprecision(3);
            std::cout << "Congratulations! You finished the level in " << totalSeconds << " seconds.\n";
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Apply post-processing
        glUseProgram(postProcessShader);
        
        int screenTextureLoc = glGetUniformLocation(postProcessShader, "screenTexture");
        glUniform1i(screenTextureLoc, 0);
        
        int timeUniformLoc = glGetUniformLocation(postProcessShader, "time");
        glUniform1f(timeUniformLoc, static_cast<float>(currentFrame));
        
        int gameWonLoc = glGetUniformLocation(postProcessShader, "gameWon");
        glUniform1i(gameWonLoc, gameWon);
        
        int winTimeLoc = glGetUniformLocation(postProcessShader, "winTime");
        glUniform1f(winTimeLoc, static_cast<float>(winTime));
        
        int animationStartTimeLoc = glGetUniformLocation(postProcessShader, "animationStartTime");
        glUniform1f(animationStartTimeLoc, static_cast<float>(animationStartTime));
        
        int animationStartedLoc = glGetUniformLocation(postProcessShader, "animationStarted");
        glUniform1i(animationStartedLoc, animationStarted);

        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    glDeleteVertexArrays(1, &rhombusVAO);
    glDeleteBuffers(1, &rhombusVBO);
    glDeleteProgram(rhombusShader);
    
    glDeleteVertexArrays(1, &finishVAO);
    glDeleteBuffers(1, &finishVBO);
    glDeleteProgram(finishShaderProgram);
    
    glDeleteVertexArrays(1, &playerVAO);
    glDeleteBuffers(1, &playerVBO);
    glDeleteProgram(playerShaderProgram);

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteTextures(1, &textureColorBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteProgram(postProcessShader);

    glfwTerminate();
    return 0;
}
