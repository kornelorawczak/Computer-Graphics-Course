#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputHandler {
public:
    InputHandler();
    
    void update(GLFWwindow* window);
    
    bool keys[GLFW_KEY_LAST] = {false};
    bool mouseButtons[GLFW_MOUSE_BUTTON_LAST] = {false};
    glm::vec2 mousePosition;
    glm::vec2 mouseDelta;
    float mouseScroll;
    
    bool forward() const { return keys[GLFW_KEY_W]; }
    bool backward() const { return keys[GLFW_KEY_S]; }
    bool left() const { return keys[GLFW_KEY_A]; }
    bool right() const { return keys[GLFW_KEY_D]; }
    bool rise() const { return keys[GLFW_KEY_E]; }        
    bool descend() const { return keys[GLFW_KEY_Q]; }     
    bool sprint() const { return keys[GLFW_KEY_LEFT_SHIFT]; }
    bool toggleTransparency() const { return keys[GLFW_KEY_T]; }
    bool switchCamera() const { return keys[GLFW_KEY_TAB]; }
    
    bool mouseLookActive() const { return true; }
private:
    double lastMouseX, lastMouseY;
    bool firstMouse;
};

#endif