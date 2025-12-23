#include "Renderer.hpp"
#include "MeshGenerator.hpp"
#include "ObstacleManager.hpp"
#include "shader_utils.h"
#include "MiniMap.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp> 
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "CoinManager.hpp"

// Handles shader rendering, the window viewport, cleanup, obstacles rendering and walls

Renderer::Renderer() 
    : obstacleShader(0), wallShader(0), sphereShader(0), wireframeShader(0), 
      obstacleVAO(0), obstacleVBO(0), obstacleEBO(0),
      cubeVAO(0), cubeVBO(0), cubeEBO(0),
      sphereVAO(0), sphereVBO(0), sphereEBO(0),
      wireframeVAO(0), wireframeVBO(0), wireframeEBO(0),
      wallTexture(0),
      viewportX(0), viewportY(0), viewportWidth(1200), viewportHeight(900) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize() {
    obstacleShader = createShaderProgram("../shaders/basic_vert.glsl", "../shaders/obstacle_dots_frag.glsl");
    wallShader = createShaderProgram("../shaders/basic_vert.glsl", "../shaders/wall_frag.glsl");
    sphereShader = createShaderProgram("../shaders/basic_vert.glsl", "../shaders/sphere_frag.glsl");
    wireframeShader = createShaderProgram("../shaders/basic_vert.glsl", "../shaders/wireframe_frag.glsl");
    coinShader = createShaderProgram("../shaders/basic_vert.glsl", "../shaders/coin_frag.glsl");

    
    if (!obstacleShader || !wallShader || !sphereShader || !wireframeShader || !coinShader) {
        std::cout << "Failed to load one or more shaders!" << std::endl;
        return false;
    }

    stbi_set_flip_vertically_on_load(true); 
    int texWidth, texHeight, texChannels;
    unsigned char* data = stbi_load("../shaders/wall_texture.JPG", &texWidth, &texHeight, &texChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (texChannels == 1) format = GL_RED;
        else if (texChannels == 3) format = GL_RGB;
        else if (texChannels == 4) format = GL_RGBA;

        glGenTextures(1, &wallTexture);
        glBindTexture(GL_TEXTURE_2D, wallTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load wall texture at: " << "../shaders/wall_texture.JPG" << std::endl;
    }
    
    // Setup meshes
    setupObstacleMesh();
    setupCubeMesh();
    setupSphereMesh();
    setupWireframeCubeMesh();
    setupCoinMesh();
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    return true;
}

void Renderer::cleanup() {
    if (obstacleVAO) glDeleteVertexArrays(1, &obstacleVAO);
    if (obstacleVBO) glDeleteBuffers(1, &obstacleVBO);
    if (obstacleEBO) glDeleteBuffers(1, &obstacleEBO);
    
    if (cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
    if (cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if (cubeEBO) glDeleteBuffers(1, &cubeEBO);
    
    if (sphereVAO) glDeleteVertexArrays(1, &sphereVAO);
    if (sphereVBO) glDeleteBuffers(1, &sphereVBO);
    if (sphereEBO) glDeleteBuffers(1, &sphereEBO);
    
    if (obstacleShader) glDeleteProgram(obstacleShader);
    if (wallShader) glDeleteProgram(wallShader);
    if (sphereShader) glDeleteProgram(sphereShader);

    if (wallTexture) glDeleteTextures(1, &wallTexture);

    if (wireframeVAO) glDeleteVertexArrays(1, &wireframeVAO);
    if (wireframeVBO) glDeleteBuffers(1, &wireframeVBO);
    if (wireframeEBO) glDeleteBuffers(1, &wireframeEBO);
    if (wireframeShader) glDeleteProgram(wireframeShader);

    if (coinVAO) glDeleteVertexArrays(1, &coinVAO);
    if (coinVBO) glDeleteBuffers(1, &coinVBO);
    if (coinEBO) glDeleteBuffers(1, &coinEBO);
    if (coinShader) glDeleteProgram(coinShader);
}

void Renderer::setViewport(int x, int y, int width, int height) {
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
    glViewport(x, y, width, height);
}

void Renderer::renderMiniMap(const MiniMap& miniMap,
                            const std::vector<Obstacle>& obstacles,
                            const std::vector<Coin>& coins,
                            const glm::vec3& playerPosition, float playerRadius) {
    
    
    int x, y, width, height;
    miniMap.getViewport(x, y, width, height);
    glViewport(x, y, width, height);
    
    // Enable scissor test to cleanly separate minimap from main view
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
    
    // Clear depth buffer for minimap area
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Get minimap view and projection matrices
    glm::mat4 view = miniMap.getViewMatrix();
    glm::mat4 projection = miniMap.getProjectionMatrix();
    
    // Render wireframe cube
    renderCubeWireframe(view, projection);
    
    // Render obstacles in minimap 
    glUseProgram(obstacleShader);
    glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glBindVertexArray(obstacleVAO);
    for (const auto& obstacle : obstacles) {
        glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "model"), 1, GL_FALSE, &obstacle.transform[0][0]);
        glUniform3f(glGetUniformLocation(obstacleShader, "objectColor"), 
                   obstacle.color.r, obstacle.color.g, obstacle.color.b);
        glDrawElements(GL_TRIANGLES, obstacleIndexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    
    // Render player in minimap
    glm::mat4 playerModel = glm::mat4(1.0f);
    playerModel = glm::translate(playerModel, playerPosition);
    playerModel = glm::scale(playerModel, glm::vec3(playerRadius));
    
    glUseProgram(sphereShader);
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "model"), 1, GL_FALSE, &playerModel[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform3f(glGetUniformLocation(sphereShader, "objectColor"), 1.0f, 1.0f, 1.0f); 
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Render coins in minimap
    glUseProgram(coinShader);
    glUniformMatrix4fv(glGetUniformLocation(coinShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(coinShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glBindVertexArray(coinVAO);
    for (const auto& coin : coins) { 
        if (!coin.collected) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coin.position);
            model = glm::scale(model, glm::vec3(coin.radius * 2.0f)); // Slightly larger in minimap
            
            glUniformMatrix4fv(glGetUniformLocation(coinShader, "model"), 1, GL_FALSE, &model[0][0]);
            glUniform3f(glGetUniformLocation(coinShader, "objectColor"), 1.0f, 1.0f, 0.0f);
            
            glDrawElements(GL_TRIANGLES, coinIndexCount, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
    
    glDisable(GL_SCISSOR_TEST);


}

void Renderer::renderCubeWireframe(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(wireframeShader);
    
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(wireframeShader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(wireframeShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(wireframeShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform3f(glGetUniformLocation(wireframeShader, "objectColor"), 1.0f, 1.0f, 0.0f); 
    
    // Use line drawing for wireframe
    glBindVertexArray(wireframeVAO);
    glDrawElements(GL_LINES, wireframeIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::setupWireframeCubeMesh() {
    Mesh wireframeCube = MeshGenerator::generateWireframeCube();
    setupMeshBuffers(wireframeCube, wireframeVAO, wireframeVBO, wireframeEBO);
    wireframeIndexCount = wireframeCube.indices.size();
}

void Renderer::clear() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderObstacles(const std::vector<Obstacle>& obstacles, 
                              const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(obstacleShader);
    
    glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glBindVertexArray(obstacleVAO);
    
    for (const auto& obstacle : obstacles) {
        glUniformMatrix4fv(glGetUniformLocation(obstacleShader, "model"), 1, GL_FALSE, &obstacle.transform[0][0]);
        glUniform3f(glGetUniformLocation(obstacleShader, "objectColor"), 
                   obstacle.color.r, obstacle.color.g, obstacle.color.b);
        
        glDrawElements(GL_TRIANGLES, obstacleIndexCount, GL_UNSIGNED_INT, 0);
    }
    
    glBindVertexArray(0);
}

void Renderer::renderCubeWalls(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(wallShader);
    
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(wallShader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(wallShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(wallShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glActiveTexture(GL_TEXTURE0);
    if (wallTexture) {
        glBindTexture(GL_TEXTURE_2D, wallTexture);
    } else {
        glBindTexture(GL_TEXTURE_2D, 0); 
    }
    GLint loc = glGetUniformLocation(wallShader, "wallTexture");
    if (loc != -1) glUniform1i(loc, 0); 

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::renderPlayerSphere(const glm::vec3& position, float radius,
                                  const glm::mat4& view, const glm::mat4& projection) {                           
    glUseProgram(sphereShader);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(radius));
    
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(sphereShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform3f(glGetUniformLocation(sphereShader, "objectColor"), 1.0f, 1.0f, 1.0f); // White sphere
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint Renderer::createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    return loadShader(vertexPath, fragmentPath);
}

void Renderer::setupMeshBuffers(const Mesh& mesh, GLuint& VAO, GLuint& VBO, GLuint& EBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), 
                 mesh.vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int),
                 mesh.indices.data(), GL_STATIC_DRAW);
    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                         (void*)offsetof(Vertex, normal));
    
    // Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                        (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
}

void Renderer::setupObstacleMesh() {
    Mesh octahedron = MeshGenerator::generateOctahedron();
    setupMeshBuffers(octahedron, obstacleVAO, obstacleVBO, obstacleEBO);
    obstacleIndexCount = octahedron.indices.size();
}

void Renderer::setupCubeMesh() {
    Mesh cube = MeshGenerator::generateCube();
    setupMeshBuffers(cube, cubeVAO, cubeVBO, cubeEBO);
    cubeIndexCount = cube.indices.size();
}

void Renderer::setupSphereMesh() {
    Mesh sphere = MeshGenerator::generateSphere();
    setupMeshBuffers(sphere, sphereVAO, sphereVBO, sphereEBO);
    sphereIndexCount = sphere.indices.size();
}

void Renderer::renderCoins(const std::vector<Coin>& coins, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(coinShader);
    
    glUniformMatrix4fv(glGetUniformLocation(coinShader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(coinShader, "projection"), 1, GL_FALSE, &projection[0][0]);
    
    glBindVertexArray(coinVAO);
    
    for (const auto& coin : coins) {
        if (!coin.collected) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coin.position);
            model = glm::scale(model, glm::vec3(coin.radius));
            
            glUniformMatrix4fv(glGetUniformLocation(coinShader, "model"), 1, GL_FALSE, &model[0][0]);
            glUniform3f(glGetUniformLocation(coinShader, "objectColor"), 1.0f, 1.0f, 0.0f); // Yellow
            
            glDrawElements(GL_TRIANGLES, coinIndexCount, GL_UNSIGNED_INT, 0);
        }
    }
    
    glBindVertexArray(0);
}

void Renderer::setupCoinMesh() {
    Mesh coinSphere = MeshGenerator::generateSphere(8, 8); // Lower resolution for performance
    setupMeshBuffers(coinSphere, coinVAO, coinVBO, coinEBO);
    coinIndexCount = coinSphere.indices.size();
}