#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MiniMap {
private:
    int viewportX, viewportY, viewportWidth, viewportHeight;
    glm::vec3 cameraPosition;
    glm::vec3 lookAtCenter;
    glm::vec3 upVector;
    float worldSize;

    void updateCameraPosition();

public:
    MiniMap();
    void update(const glm::vec3& playerPos, const glm::vec3& volcanoPos);
    void render();
    void setWindowSize(int width, int height);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    void getViewport(int& x, int& y, int& width, int& height) const;
    
    float getBladeRotation() const { return 0.0f; } 
};

#endif