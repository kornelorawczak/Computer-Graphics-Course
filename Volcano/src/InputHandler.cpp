#include "InputHandler.hpp"

InputHandler::InputHandler() : lastMouseX(0.0), lastMouseY(0.0), firstMouse(true) {
    mousePosition = glm::vec2(0.0f);
    mouseDelta = glm::vec2(0.0f);
    mouseScroll = 0.0f;
}

void InputHandler::update(GLFWwindow* window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }
    
    mouseDelta.x = xpos - lastMouseX;
    mouseDelta.y = lastMouseY - ypos; 
    
    lastMouseX = xpos;
    lastMouseY = ypos;
    mousePosition = glm::vec2(xpos, ypos);
    
    for (int key = 0; key < GLFW_KEY_LAST; key++) {
        keys[key] = (glfwGetKey(window, key) == GLFW_PRESS);
    }
    
    for (int button = 0; button < GLFW_MOUSE_BUTTON_LAST; button++) {
        mouseButtons[button] = (glfwGetMouseButton(window, button) == GLFW_PRESS);
    }
    
    mouseScroll = 0.0f;
}