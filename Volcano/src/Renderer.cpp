#include "Renderer.hpp"
#include "shader_utils.h"
#include "Camera.hpp"
#include "Player.hpp"
#include "BubbleManager.hpp"
#include "MeshGenerator.hpp"
#include "MiniMap.hpp"
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() : 
    useTransparency(true), 
    useFog(true), 
    useFlashlight(true), 
    fogDensity(0.000001f), 
    fogColor(glm::vec3(0.7f, 0.7f, 0.7f)),
    windowWidth(1200), 
    windowHeight(800), 
    sphereMesh(nullptr),
    bladeMesh(nullptr),
    arrowMesh(nullptr),
    lastTime(0.0), 
    frameCount(0), 
    fps(0.0) {
}

Renderer::~Renderer() {
    cleanup();
}

bool Renderer::initialize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    loadShaders();
    
    sphereMesh = new Mesh(MeshGenerator::createSphere(16, 16));
    sphereMesh->setupMesh();

    bladeMesh = new Mesh(MeshGenerator::createHelicopterBlade());
    bladeMesh->setupMesh();

    arrowMesh = new Mesh(MeshGenerator::createArrowTriangle());
    arrowMesh->setupMesh();

    setupAnimation();
    
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * 5000, nullptr, GL_DYNAMIC_DRAW); 
    
    glBindVertexArray(sphereMesh->VAO);
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0);
    glVertexAttribDivisor(2, 1);
    
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, scale));
    glVertexAttribDivisor(3, 1);
    
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
    glVertexAttribDivisor(4, 1);
    
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, alpha));
    glVertexAttribDivisor(5, 1);
    
    glBindVertexArray(0);
    
    lastTime = glfwGetTime();
    
    return true;
}



void Renderer::cleanup() {
    if (sphereMesh) {
        sphereMesh->cleanup();
        delete sphereMesh;
        sphereMesh = nullptr;
    }

    if (bladeMesh) {
        bladeMesh->cleanup();
        delete bladeMesh;
        bladeMesh = nullptr;
    }

    if (arrowMesh) {
        arrowMesh->cleanup();
        delete arrowMesh;
        arrowMesh = nullptr;
    }
    
    cleanupAnimation();

    glDeleteBuffers(1, &instanceVBO);
    
    glDeleteProgram(phongShader);
    glDeleteProgram(instancedSphereShader);
    glDeleteProgram(minimapShader);
    glDeleteProgram(uiShader);
}

void Renderer::render(const Camera& camera, const Player& player, const BubbleManager& bubbleManager, 
                     const Mesh& terrain, const Mesh& ground, const Mesh& walls,
                     const MiniMap& miniMap, float gameTime, int currentLevel) {
    
    double currentTime = glfwGetTime();
    frameCount++;
    if (currentTime - lastTime >= 1.0) {
        fps = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }

    useFlashlight = true;
    
    renderOpaque(camera, player, terrain, ground, walls);

    renderHelicopterBlades(camera, player);
    if (camera.currentMode == THIRD_PERSON) {
        renderPlayer(camera, player);
    }
    
    renderTransparentBubbles(camera, player, bubbleManager);

    renderMiniMap(miniMap, player, terrain);
    
    renderUI(currentLevel, gameTime);
}

void Renderer::renderHelicopterBlades(const Camera& camera, const Player& player) {
    glUseProgram(phongShader);
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    GLuint modelLoc = glGetUniformLocation(phongShader, "u_Model");
    GLuint viewLoc = glGetUniformLocation(phongShader, "u_View");
    GLuint projLoc = glGetUniformLocation(phongShader, "u_Proj");
    GLuint objectColorLoc = glGetUniformLocation(phongShader, "u_ObjectColor");
    
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLuint shininessLoc = glGetUniformLocation(phongShader, "u_Shininess");
    GLuint specularStrengthLoc = glGetUniformLocation(phongShader, "u_SpecularStrength");
    if (shininessLoc != -1) glUniform1f(shininessLoc, 128.0f);
    if (specularStrengthLoc != -1) glUniform1f(specularStrengthLoc, 1.0f);
    
    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 1.0f, 0.84f, 0.0f); 
    
    float bladeLength = 8.0f;
    float bladeWidth = 1.0f;
    float bladeHeight = 0.1f;
    
    glm::vec3 bladePosition = player.position + glm::vec3(0.0f, 2.0f, 0.0f);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, bladePosition);
    model = glm::rotate(model, glm::radians(player.getBladeRotation()), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(bladeWidth, bladeHeight, bladeLength));
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    if (bladeMesh) {
        bladeMesh->draw();
    }
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, bladePosition);
    model = glm::rotate(model, glm::radians(player.getBladeRotation() + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(bladeWidth, bladeHeight, bladeLength));
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    if (bladeMesh) {
        bladeMesh->draw();
    }
}

void Renderer::renderMiniMap(const MiniMap& miniMap, const Player& player, const Mesh& terrain) {
    int x, y, width, height;
    miniMap.getViewport(x, y, width, height);
    
    glViewport(x, y, width, height);
    
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(minimapShader);
    
    glm::mat4 view = miniMap.getViewMatrix();
    glm::mat4 projection = miniMap.getProjectionMatrix();
    
    GLuint modelLoc = glGetUniformLocation(minimapShader, "u_Model");
    GLuint viewLoc = glGetUniformLocation(minimapShader, "u_View");
    GLuint projLoc = glGetUniformLocation(minimapShader, "u_Proj");
    GLuint colorLoc = glGetUniformLocation(minimapShader, "u_Color");
    
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    glm::mat4 model = glm::mat4(1.0f);
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (colorLoc != -1) glUniform3f(colorLoc, 0.4f, 0.3f, 0.2f); // Brown for volcano
    
    terrain.draw();
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, player.position);
    model = glm::scale(model, glm::vec3(100.0f)); 
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (colorLoc != -1) glUniform3f(colorLoc, 1.0f, 0.84f, 0.0f); 
    
    if (sphereMesh) {
        sphereMesh->draw();
    }
    
    if (bladeMesh) {
        float bladeScale = 25.0f; 
        float bladeLength = 8.0f * bladeScale;
        float bladeWidth = 1.0f * bladeScale;
        
        glm::vec3 bladePosition = player.position;
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, bladePosition);
        model = glm::rotate(model, glm::radians(player.getBladeRotation()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(bladeWidth, 1.0f, bladeLength)); 
        
        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        if (colorLoc != -1) glUniform3f(colorLoc, 1.0f, 0.84f, 0.0f); 
        
        bladeMesh->draw();
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, bladePosition);
        model = glm::rotate(model, glm::radians(player.getBladeRotation() + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(bladeWidth, 1.0f, bladeLength));
        
        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        if (colorLoc != -1) glUniform3f(colorLoc, 1.0f, 0.84f, 0.0f);
        
        bladeMesh->draw();
    }

    if (arrowMesh) {
        glm::vec3 lookDirection = player.front;
        
        float angle = atan2(lookDirection.x, lookDirection.z);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, player.position);
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(350.0f)); 
        
        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        if (colorLoc != -1) glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); 
        
        arrowMesh->draw();
    }
    
    glDisable(GL_SCISSOR_TEST);
    
    glViewport(0, 0, windowWidth, windowHeight);
}

void Renderer::loadShaders() {
    phongShader = ::loadShader("../shaders/vertex_phong.glsl", "../shaders/fragment_phong.glsl");
    instancedSphereShader = ::loadShader("../shaders/vertex_instanced_sphere.glsl", "../shaders/fragment_instanced_sphere.glsl");
    minimapShader = ::loadShader("../shaders/vertex_minimap.glsl", "../shaders/fragment_minimap.glsl");
    uiShader = ::loadShader("../shaders/vertex_ui.glsl", "../shaders/fragment_ui.glsl");
    animationShader = ::loadShader("../shaders/vertex_animation.glsl", "../shaders/fragment_animation.glsl");
}

void Renderer::renderOpaque(const Camera& camera, const Player& player, const Mesh& terrain, const Mesh& ground, const Mesh& walls) {
    glUseProgram(phongShader);
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    GLuint modelLoc = glGetUniformLocation(phongShader, "u_Model");
    GLuint viewLoc = glGetUniformLocation(phongShader, "u_View");
    GLuint projLoc = glGetUniformLocation(phongShader, "u_Proj");
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLuint lightDirLoc = glGetUniformLocation(phongShader, "u_LightDir");
    GLuint viewPosLoc = glGetUniformLocation(phongShader, "u_ViewPos");
    GLuint lightColorLoc = glGetUniformLocation(phongShader, "u_LightColor");
    GLuint objectColorLoc = glGetUniformLocation(phongShader, "u_ObjectColor");
    GLuint shininessLoc = glGetUniformLocation(phongShader, "u_Shininess");
    GLuint specularStrengthLoc = glGetUniformLocation(phongShader, "u_SpecularStrength");
    
    if (lightDirLoc != -1) glUniform3f(lightDirLoc, 0.0f, -1.0f, 0.0f);
    if (viewPosLoc != -1) glUniform3f(viewPosLoc, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    if (lightColorLoc != -1) glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    if (shininessLoc != -1) glUniform1f(shininessLoc, 32.0f);
    if (specularStrengthLoc != -1) glUniform1f(specularStrengthLoc, 0.5f);
    
    GLuint fogColorLoc = glGetUniformLocation(phongShader, "u_FogColor");
    GLuint fogDensityLoc = glGetUniformLocation(phongShader, "u_FogDensity");
    
    if (fogColorLoc != -1) glUniform3f(fogColorLoc, fogColor.r, fogColor.g, fogColor.b);
    if (fogDensityLoc != -1) glUniform1f(fogDensityLoc, useFog ? fogDensity : 0.0f);
    
    GLuint useFlashlightLoc = glGetUniformLocation(phongShader, "u_UseFlashlight");
    if (useFlashlightLoc != -1) glUniform1i(useFlashlightLoc, useFlashlight);
    
    GLuint flashlightPosLoc = glGetUniformLocation(phongShader, "u_FlashlightPos");
    GLuint flashlightDirLoc = glGetUniformLocation(phongShader, "u_FlashlightDir");
    GLuint flashlightCutoffLoc = glGetUniformLocation(phongShader, "u_FlashlightCutoff");
    GLuint flashlightOuterCutoffLoc = glGetUniformLocation(phongShader, "u_FlashlightOuterCutoff");
    GLuint flashlightColorLoc = glGetUniformLocation(phongShader, "u_FlashlightColor");
    
    glm::vec3 flashlightPos = player.getFlashlightPosition();
    
    glm::vec3 flashlightDir = player.getFlashlightDirection();
    
    
    if (flashlightPosLoc != -1) glUniform3f(flashlightPosLoc, flashlightPos.x, flashlightPos.y, flashlightPos.z);
    if (flashlightDirLoc != -1) glUniform3f(flashlightDirLoc, flashlightDir.x, flashlightDir.y, flashlightDir.z);
    if (flashlightCutoffLoc != -1) glUniform1f(flashlightCutoffLoc, glm::cos(glm::radians(25.0f)));
    if (flashlightOuterCutoffLoc != -1) glUniform1f(flashlightOuterCutoffLoc, glm::cos(glm::radians(50.0f))); 
    if (flashlightColorLoc != -1) glUniform3f(flashlightColorLoc, 0.0f, 0.8f, 1.0f); 
    
    
    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 0.0f, 0.6f, 0.0f); 
    ground.draw();

    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 0.5f, 0.5f, 0.5f); 
    walls.draw();
    
    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 0.4f, 0.3f, 0.2f); 
    terrain.draw();
}

void Renderer::setupAnimation() {
    float quadVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
        
        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &animationVAO);
    glGenBuffers(1, &animationVBO);
    
    glBindVertexArray(animationVAO);
    glBindBuffer(GL_ARRAY_BUFFER, animationVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void Renderer::cleanupAnimation() {
    if (animationVAO) glDeleteVertexArrays(1, &animationVAO);
    if (animationVBO) glDeleteBuffers(1, &animationVBO);
}

void Renderer::renderAnimation(float progress, const glm::vec3& color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDisable(GL_DEPTH_TEST);
    
    glUseProgram(animationShader);
    
    GLint progressLoc = glGetUniformLocation(animationShader, "progress");
    GLint resolutionLoc = glGetUniformLocation(animationShader, "resolution");
    GLint colorLoc = glGetUniformLocation(animationShader, "color");
    
    glUniform1f(progressLoc, progress);
    glUniform2f(resolutionLoc, (float)windowWidth, (float)windowHeight);
    glUniform3f(colorLoc, color.r, color.g, color.b);
    
    glBindVertexArray(animationVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void Renderer::renderTransparentBubbles(const Camera& camera, const Player& player, const BubbleManager& bubbleManager) {
    if (!useTransparency) {
        return;
    }
    
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE); 
    
    glUseProgram(instancedSphereShader);
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    GLuint viewLoc = glGetUniformLocation(instancedSphereShader, "u_View");
    GLuint projLoc = glGetUniformLocation(instancedSphereShader, "u_Proj");
    
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLuint lightDirLoc = glGetUniformLocation(instancedSphereShader, "u_LightDir");
    GLuint viewPosLoc = glGetUniformLocation(instancedSphereShader, "u_ViewPos");
    GLuint lightColorLoc = glGetUniformLocation(instancedSphereShader, "u_LightColor");
    GLuint shininessLoc = glGetUniformLocation(instancedSphereShader, "u_Shininess");
    GLuint specularStrengthLoc = glGetUniformLocation(instancedSphereShader, "u_SpecularStrength");
    
    if (lightDirLoc != -1) glUniform3f(lightDirLoc, 0.0f, -1.0f, 0.0f);
    if (viewPosLoc != -1) glUniform3f(viewPosLoc, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    if (lightColorLoc != -1) glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
    if (shininessLoc != -1) glUniform1f(shininessLoc, 128.0f);
    if (specularStrengthLoc != -1) glUniform1f(specularStrengthLoc, 1.0f);
    
    GLuint fogColorLoc = glGetUniformLocation(instancedSphereShader, "u_FogColor");
    GLuint fogDensityLoc = glGetUniformLocation(instancedSphereShader, "u_FogDensity");
    
    if (fogColorLoc != -1) glUniform3f(fogColorLoc, fogColor.r, fogColor.g, fogColor.b);
    if (fogDensityLoc != -1) glUniform1f(fogDensityLoc, useFog ? fogDensity : 0.0f);

    GLuint useFlashlightLoc = glGetUniformLocation(instancedSphereShader, "u_UseFlashlight");
    GLuint flashlightPosLoc = glGetUniformLocation(instancedSphereShader, "u_FlashlightPos");
    GLuint flashlightDirLoc = glGetUniformLocation(instancedSphereShader, "u_FlashlightDir");
    GLuint flashlightCutoffLoc = glGetUniformLocation(instancedSphereShader, "u_FlashlightCutoff");
    GLuint flashlightOuterCutoffLoc = glGetUniformLocation(instancedSphereShader, "u_FlashlightOuterCutoff");
    GLuint flashlightColorLoc = glGetUniformLocation(instancedSphereShader, "u_FlashlightColor");
    
    if (useFlashlightLoc != -1) glUniform1i(useFlashlightLoc, useFlashlight);
    
    glm::vec3 flashlightPos = player.getFlashlightPosition();
    
    glm::vec3 flashlightDir = player.getFlashlightDirection();
    
    
    if (flashlightPosLoc != -1) glUniform3f(flashlightPosLoc, flashlightPos.x, flashlightPos.y, flashlightPos.z);
    if (flashlightDirLoc != -1) glUniform3f(flashlightDirLoc, flashlightDir.x, flashlightDir.y, flashlightDir.z);
    if (flashlightCutoffLoc != -1) glUniform1f(flashlightCutoffLoc, glm::cos(glm::radians(25.0f)));
    if (flashlightOuterCutoffLoc != -1) glUniform1f(flashlightOuterCutoffLoc, glm::cos(glm::radians(50.0f)));
    if (flashlightColorLoc != -1) glUniform3f(flashlightColorLoc, 0.0f, 0.8f, 1.0f); // Bright blue
    
    
    auto aliveBubbles = bubbleManager.getAliveBubbles();
    
    std::sort(aliveBubbles.begin(), aliveBubbles.end(), 
        [&camera](const Bubble& a, const Bubble& b) {
            float distA = glm::length(camera.getPosition() - a.position);
            float distB = glm::length(camera.getPosition() - b.position);
            return distA > distB;
        });
    
    bubbleInstanceData.clear();
    for (const auto& bubble : aliveBubbles) {
        InstanceData instance;
        instance.position = bubble.position;
        instance.scale = bubble.currentRadius;
        instance.color = bubble.color;
        instance.alpha = 0.7f; 
        bubbleInstanceData.push_back(instance);
    }
    
    if (!bubbleInstanceData.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bubbleInstanceData.size() * sizeof(InstanceData), bubbleInstanceData.data());
    }
    
    if (sphereMesh) {
        glBindVertexArray(sphereMesh->VAO);
        glDrawElementsInstanced(GL_TRIANGLES, sphereMesh->indices.size(), GL_UNSIGNED_INT, 0, bubbleInstanceData.size());
        glBindVertexArray(0);
    }
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Renderer::renderPlayer(const Camera& camera, const Player& player) {
    glUseProgram(phongShader);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, player.position);
    model = glm::scale(model, glm::vec3(2.0f)); 
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    GLuint modelLoc = glGetUniformLocation(phongShader, "u_Model");
    GLuint viewLoc = glGetUniformLocation(phongShader, "u_View");
    GLuint projLoc = glGetUniformLocation(phongShader, "u_Proj");
    GLuint objectColorLoc = glGetUniformLocation(phongShader, "u_ObjectColor");
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    if (objectColorLoc != -1) glUniform3f(objectColorLoc, 1.0f, 0.84f, 0.0f); 
    GLuint shininessLoc = glGetUniformLocation(phongShader, "u_Shininess");
    GLuint specularStrengthLoc = glGetUniformLocation(phongShader, "u_SpecularStrength");
    if (shininessLoc != -1) glUniform1f(shininessLoc, 128.0f);
    if (specularStrengthLoc != -1) glUniform1f(specularStrengthLoc, 1.0f);

    if (sphereMesh) {
        sphereMesh->draw();
    }
}

void Renderer::renderUI(int currentLevel, float gameTime) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(uiShader);    
    glEnable(GL_DEPTH_TEST);
}

void Renderer::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}
