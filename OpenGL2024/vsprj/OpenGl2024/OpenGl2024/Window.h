#ifndef WINDOW_H
#define WINDOW_H

#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    void processInput(WorldInformation& worldInfo);
    bool shouldClose() const { return glfwWindowShouldClose(window); }
    void swapBuffers() { glfwSwapBuffers(window); }
    void pollEvents() { glfwPollEvents(); }
    GLFWwindow* getGLFWwindow() const { return window; }

    glm::vec3 getCameraPosition() const { return cameraPos; }
    glm::vec3 getCameraFront() const { return cameraFront; }
    glm::vec3 getCameraUp() const { return cameraUp; }

    static float cameraSpeed;

private:
    GLFWwindow* window;

    WorldInformation* worldInformation;

    static bool keys[1024];
    static double lastX, lastY;
    static bool firstMouse;
    static float yaw, pitch;
    static glm::vec3 cameraPos, cameraFront, cameraUp;
    static glm::quat camQuaternion;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    void updateCameraVectors();
};

#endif
