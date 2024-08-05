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
    void setupShaders();  // Helper function to configure shaders
    void render(GLFWwindow* window); // Render function

    void CreateProgram(GLuint& programID, const char* vertexSource, const char* fragmentSource);

private:
    GLuint programID, skyProgramID;     // OpenGL program IDs
    GLuint textureID;     // Texture ID

    GLuint vertex_buffer, index_buffer, vertex_array;
    GLuint vertex_shader, fragment_shader;

    void processInput(GLFWwindow* window);
    GLuint dirt, sand, grass, rock, snow; // Texture IDs

    void setupTextures();
    GLuint compileShader(GLenum type, const char* source);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    void processUniforms(GLuint program);
    std::string readFile(const std::string& filePath);

    void setupVertexArray(GLuint programID);
    void setupUniforms(GLuint programID);
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

#endif