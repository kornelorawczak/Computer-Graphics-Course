#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <GLFW/glfw3.h>

class InputHandler {
public:
    InputHandler();
    
    void update(GLFWwindow* window);
    void processMouseMovement(double xpos, double ypos);
    void processKey(int key, int action);
    
    // Getters
    bool isKeyPressed(int key) const { return keys[key]; }
    bool isKeyJustPressed(int key) const { return keys[key] && !prevKeys[key]; }
    float getMouseXOffset() const { return mouseXOffset; }
    float getMouseYOffset() const { return mouseYOffset; }
    bool isMouseCaptured() const { return mouseCaptured; }
    
    void resetMouseOffset() { mouseXOffset = mouseYOffset = 0.0f; }
    void endFrame();
private:
    bool keys[1024];
    bool prevKeys[1024];
    float mouseXOffset;
    float mouseYOffset;
    bool firstMouse;
    double lastX;
    double lastY;
    bool mouseCaptured;
};

#endif