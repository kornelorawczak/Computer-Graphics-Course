#include "InputHandler.hpp"

InputHandler::InputHandler() 
    : mouseXOffset(0.0f), mouseYOffset(0.0f), firstMouse(true), 
      lastX(0.0), lastY(0.0), mouseCaptured(true) {
    for (int i = 0; i < 1024; i++) {
        keys[i] = false;
        prevKeys[i] = false;
    }
}

void InputHandler::update(GLFWwindow* window) {
    resetMouseOffset();
    
    if (mouseCaptured) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void InputHandler::processMouseMovement(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    if (mouseCaptured) {
        mouseXOffset = xpos - lastX;
        mouseYOffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    }
    
    lastX = xpos;
    lastY = ypos;
}

void InputHandler::processKey(int key, int action) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
            
            if (key == GLFW_KEY_ESCAPE) {
                mouseCaptured = !mouseCaptured;
            }
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void InputHandler::endFrame() {
    // Copy current key states to previous states
    for (int i = 0; i < 1024; i++) {
        prevKeys[i] = keys[i];
    }
}