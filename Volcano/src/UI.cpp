#include "UI.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>

UI::UI() : windowWidth(1200), windowHeight(800) {
}

void UI::render(int currentLevel, float gameTime, int lives, const glm::vec3& playerPos) {
    
}

void UI::renderGameOver(int finalLevel, float totalTime) {
    
}

void UI::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    
}

void UI::renderBar(float x, float y, float width, float height, float value, const glm::vec3& color) {
    
}