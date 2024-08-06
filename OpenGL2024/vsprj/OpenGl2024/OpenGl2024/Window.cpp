#include <glad/glad.h>
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>


// Initialiseer statische variabelen
bool Window::keys[1024] = { false };
double Window::lastX = 400, Window::lastY = 300;
bool Window::firstMouse = true;
float Window::yaw = -90.0f; // Yaw wordt normaal ingesteld op -90.0 graden omdat we kijken naar negatieve z-as
float Window::pitch = 0.0f;
glm::vec3 Window::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Window::cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Window::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

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
}

Window::~Window()
{
    glfwTerminate();
}

void Window::processInput()
{
    float cameraSpeed = 0.05f;
    if (keys[GLFW_KEY_W])
        cameraPos += cameraSpeed * cameraFront; std::cout << "W werkt" << std::endl;
    if (keys[GLFW_KEY_S])
        cameraPos -= cameraSpeed * cameraFront;
    if (keys[GLFW_KEY_A])
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_D])
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        keys[key] = false;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Omgekeerd omdat y-coördinaten van boven naar beneden gaan
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}
