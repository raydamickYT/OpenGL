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
    unsigned int loadTexture(const std::string& url, int comp);
    GLuint createTriangle();  // Functie om een triangle te maken en het programma ID te retourneren
    void setupShaders();  // Hulpfunctie om shaders te configureren
    void render(); // Render functie

private:
    GLFWwindow* window;
    GLuint programID;     // OpenGL programma ID
    GLuint textureID;     // Texture ID

    void processInput(GLFWwindow* window);
    GLuint dirt, sand, grass, rock, snow; // Texture IDs

    void setupTextures();
    void processUniforms(GLuint program);
};

static const float cubeVertices[] = {
    // positions            //colors            // tex coords   // normals

    // Bottom face
    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, -1.f, 0.f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, -1.f, 0.f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, -1.f, 0.f,
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, -1.f, 0.f,

    // Right face
    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       1.f, 0.f, 0.f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   1.f, 0.f,       1.f, 0.f, 0.f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 1.f,       1.f, 0.f, 0.f,
    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       1.f, 0.f, 0.f,

    // Front face
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, 1.f,
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, 1.f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, 1.f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, 1.f,

    // Left face
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       -1.f, 0.f, 0.f,
    -0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       -1.f, 0.f, 0.f,
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   1.f, 1.f,       -1.f, 0.f, 0.f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       -1.f, 0.f, 0.f,

    // Back face
    -0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 0.f, -1.f,
    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 0.f, -1.f,
    0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 0.f, -1.f,
    -0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 0.f, -1.f,

    // Top face
    0.5f, 0.5f, -0.5f,      1.0f, 1.0f, 1.0f,   1.f, 0.f,       0.f, 1.f, 0.f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f, 1.0f,   0.f, 0.f,       0.f, 1.f, 0.f,
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f, 1.0f,   0.f, 1.f,       0.f, 1.f, 0.f,
    -0.5f, 0.5f, -0.5f,     1.0f, 1.0f, 1.0f,   1.f, 1.f,       0.f, 1.f, 0.f
};



static const unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0,   // Bottom face
    4, 5, 6, 6, 7, 4,   // Right face
    8, 9, 10, 10, 11, 8, // Front face
    12, 13, 14, 14, 15, 12, // Left face
    16, 17, 18, 18, 19, 16, // Back face
    20, 21, 22, 22, 23, 20  // Top face
};


static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 vPos;\n"
"layout(location = 1) in vec3 vCol;\n"
"layout(location = 2) in vec2 vUv;\n"
"layout(location = 3) in vec3 vNormal;\n"
"out vec2 TexCoord;\n"
"uniform mat4 MVP;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    TexCoord = vUv;\n"
"}\n";


static const char* fragment_shader_text =
"#version 330 core\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, TexCoord);\n"
"}\n";


#endif
