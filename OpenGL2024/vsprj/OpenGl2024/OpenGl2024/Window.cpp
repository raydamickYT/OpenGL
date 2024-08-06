#include <glad/glad.h>
#include "Window.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm> // Voor std::min en std::max
#include <iostream>  // Voor console output

// Initialiseer statische variabelen
bool Window::keys[1024] = { false };
double Window::lastX = 400, Window::lastY = 300;
bool Window::firstMouse = true;
float Window::yaw = -90.0f; // Yaw wordt normaal ingesteld op -90.0 graden omdat we kijken naar negatieve z-as
float Window::pitch = 0.0f;
float Window::cameraSpeed = 1.0f;
glm::vec3 Window::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Window::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Window::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::quat Window::camQuaternion = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0.0f));

Window::Window(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cout << "Failed to Initialize GLFW." << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to load GLAD" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    // Koppel dit Window-object aan het GLFW-window
    glfwSetWindowUserPointer(window, this);
}

Window::~Window()
{
    glfwTerminate();
}

void Window::processInput(WorldInformation& worldInfo)
{
    worldInformation = &worldInfo; //geef de world info door zodat we de camera kunnen updaten

    bool camChanged = false;
    if (keys[GLFW_KEY_W]) // FORWARDS
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(0, 0, 1) * cameraSpeed;
        camChanged = true;
    }
    if (keys[GLFW_KEY_S]) // BACKWARDS
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(0, 0, -1) * cameraSpeed;
        camChanged = true;
    }
    if (keys[GLFW_KEY_A]) // LEFT
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(1, 0, 0) * cameraSpeed;
        camChanged = true; 
    }
    if (keys[GLFW_KEY_D]) // RIGHT
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(-1, 0, 0) * cameraSpeed;
        camChanged = true;
    }
    if (keys[GLFW_KEY_E] || keys[GLFW_KEY_SPACE]) // UP
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(0, 1, 0) * cameraSpeed;
        camChanged = true;
    }
    if (keys[GLFW_KEY_Q] || keys[GLFW_KEY_LEFT_CONTROL]) // DOWN
    {
        worldInfo.cameraPosition += camQuaternion * glm::vec3(0, -1, 0) * cameraSpeed;
        camChanged = true;
    }
    if (keys[GLFW_KEY_LEFT_SHIFT]) // SPEED UP
    {
        cameraSpeed = 4; //verhoog de beweeg snelheid
        //cout << "shift pressed" << endl;
    }
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		cameraSpeed = 1;
        //cout << "shift released" << endl;
	}

    if (camChanged)
    {
        glm::vec3 camForward = camQuaternion * glm::vec3(0, 0, 1);
        glm::vec3 camUp = camQuaternion * glm::vec3(0, 1, 0);
        worldInfo.view = glm::lookAt(worldInfo.cameraPosition, worldInfo.cameraPosition + camForward, camUp);
    }


    // debug-uitvoer om de camerabewegingen te controleren
    //std::cout << "Camera Position: " << worldInfo.cameraPosition.x << ", " << worldInfo.cameraPosition.y << ", " << worldInfo.cameraPosition.z << std::endl;
}


//functie wordt alleen gecalled als er een key wordt ingedrukt
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (action == GLFW_PRESS)
    {
        thisWindow->keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        thisWindow->keys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

//functie wordt alleen gecalled als de muis beweegt
void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    Window* thisWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (thisWindow->firstMouse)
    {
        thisWindow->lastX = xpos;
        thisWindow->lastY = ypos;
        thisWindow->firstMouse = false;
    }

    float xoffset = xpos - thisWindow->lastX;
    float yoffset = thisWindow->lastY - ypos; // Omgekeerd omdat y-coördinaten van boven naar beneden gaan
    thisWindow->lastX = xpos;
    thisWindow->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    thisWindow->yaw -= xoffset; //inverse
    thisWindow->pitch -= yoffset; // Inverse de pitch

    thisWindow->pitch = std::min(89.9f, std::max(-89.9f, thisWindow->pitch));

    thisWindow->camQuaternion = glm::quat(glm::vec3(glm::radians(thisWindow->pitch), glm::radians(thisWindow->yaw), 0.0f));
    thisWindow->updateCameraVectors(); // update de mouse movement iedere frame

}

void Window::updateCameraVectors()
{
    glm::vec3 front;
    front.x = 2.0f * (camQuaternion.x * camQuaternion.z + camQuaternion.w * camQuaternion.y);
    front.y = 2.0f * (camQuaternion.y * camQuaternion.z - camQuaternion.w * camQuaternion.x);
    front.z = 1.0f - 2.0f * (camQuaternion.x * camQuaternion.x + camQuaternion.y * camQuaternion.y);
    cameraFront = glm::normalize(front);

    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Up vector opnieuw berekenen (indien nodig)
    worldInformation->view = glm::lookAt(worldInformation->cameraPosition, worldInformation->cameraPosition + cameraFront, cameraUp);
}


