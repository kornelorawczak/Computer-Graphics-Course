#ifndef UI_HPP
#define UI_HPP

#include <glm/glm.hpp>
#include <string>

class UI {
public:
    UI();
    
    void render(int currentLevel, float gameTime, int lives, const glm::vec3& playerPos);
    void renderGameOver(int finalLevel, float totalTime);
    
    void setWindowSize(int width, int height) { windowWidth = width; windowHeight = height; }
    
private:
    int windowWidth, windowHeight;
    
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void renderBar(float x, float y, float width, float height, float value, const glm::vec3& color);
};

#endif