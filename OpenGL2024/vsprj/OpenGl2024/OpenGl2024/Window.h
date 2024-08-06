#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    void processInput();
    bool shouldClose() const { return glfwWindowShouldClose(window); }
    void swapBuffers() { glfwSwapBuffers(window); }
    void pollEvents() { glfwPollEvents(); }
    GLFWwindow* getGLFWwindow() const { return window; }

    glm::vec3 getCameraPosition() const { return cameraPos; }
    glm::vec3 getCameraFront() const { return cameraFront; }
    glm::vec3 getCameraUp() const { return cameraUp; }

private:
    GLFWwindow* window;
    static bool keys[1024];
    static double lastX, lastY;
    static bool firstMouse;
    static float yaw, pitch;
    static glm::vec3 cameraPos, cameraFront, cameraUp;
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};

#endif
