#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <fstream>
#include <string>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h> 
#include <math/linmath.h>

class Triangle
{
public:
    Triangle();   // Constructor
    ~Triangle();  // Destructor

    GLuint createTriangle();  // Functie om een triangle te maken en het programma ID te retourneren
    void setupShaders();  // Hulpfunctie om shaders te configureren

private:
    GLFWwindow* window;

    void processInput(GLFWwindow* window);
    GLuint programID;     // OpenGL programma ID
};

static const float cubeVertices[] = {
    // positions          // colors
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, // Back face
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, // Front face
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f
};

static const unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0, // Back face
    4, 5, 6, 6, 7, 4, // Front face
    4, 5, 1, 1, 0, 4, // Bottom face
    7, 6, 2, 2, 3, 7, // Top face
    4, 7, 3, 3, 0, 4, // Left face
    5, 6, 2, 2, 1, 5  // Right face
};

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 vPos;\n"
"layout(location = 1) in vec3 vCol;\n"
"out vec3 color;\n"
"uniform mat4 MVP;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";

#endif
