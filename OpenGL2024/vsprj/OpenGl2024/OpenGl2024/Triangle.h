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

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";
#endif


