#ifndef MINIMAP_HPP
#define MINIMAP_HPP

#include <glm/glm.hpp>
#include "Camera.hpp"

class MiniMap {
public:
    enum class ViewSide {
        FRONT,  
        SIDE,     
    };

    MiniMap();
    
    void update(float deltaTime);
    void switchViewSide(ViewSide newSide);
    void cycleViewSide();
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    ViewSide getCurrentViewSide() const { return currentViewSide; }
    const Camera& getCamera() const { return camera; }
    
    void setViewport(int x, int y, int width, int height);
    void getViewport(int& x, int& y, int& width, int& height) const;

private:
    Camera camera;
    ViewSide currentViewSide;
    
    int viewportX, viewportY, viewportWidth, viewportHeight;
    
    void updateCameraPosition();
    
    // Camera positions for different views
    const glm::vec3 frontPosition = glm::vec3(0.0f, 0.0f, 1.0f);
    const glm::vec3 sidePosition = glm::vec3(-1.0f, 0.0f, 0.0f);
    
    const glm::vec3 lookAtCenter = glm::vec3(0.0f, 0.0f, 0.0f);
};

#endif