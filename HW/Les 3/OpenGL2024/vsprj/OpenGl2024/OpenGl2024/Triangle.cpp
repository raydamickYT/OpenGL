#include "Triangle.h"
#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Triangle::Triangle()
{
}

Triangle::~Triangle()
{
}

void Triangle::setupTextures() {
    dirt = loadTexture("Textures/BlockText.jpg", 0);
    //sand = loadTexture("Textures/sand.jpg", 0);
    //grass = loadTexture("Textures/grass.png", 4);
    //rock = loadTexture("Textures/rock.jpg", 0);
    //snow = loadTexture("Textures/snow.jpg", 0);
}

void Triangle::processUniforms(GLuint program) {
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "ourTexture"), 0);
    //glUniform1i(glGetUniformLocation(program, "sand"), 1);
    //glUniform1i(glGetUniformLocation(program, "grass"), 2);
    //glUniform1i(glGetUniformLocation(program, "rock"), 3);
    //glUniform1i(glGetUniformLocation(program, "snow"), 4);
}

unsigned int Triangle::loadTexture(const std::string& url, int comp) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char* data = stbi_load(url.c_str(), &width, &height, &numChannels, comp);

    if (data) {
        std::cout << "Loaded texture data: " << static_cast<void*>(data) << std::endl;
        std::cout << "Texture width: " << width << ", height: " << height << ", channels: " << numChannels << std::endl;

        if (comp != 0) numChannels = comp;

        switch (numChannels) {
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        default:
            std::cerr << "Unsupported number of channels: " << numChannels << std::endl;
            stbi_image_free(data);
            return 0;
        }

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error uploading texture data to GPU: " << error << std::endl;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "Error generating mipmaps: " << error << std::endl;
        }

        std::cout << "Texture loaded successfully: " << url << std::endl;
    }
    else {
        std::cerr << "Failed to load texture: " << url << std::endl;
        return 0;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

std::string Triangle::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Triangle::setupShaders()
{
    glEnable(GL_DEPTH_TEST);

    GLuint vertex_buffer, index_buffer;
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &index_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    const std::string vertexShaderSource = readFile("simpleVertex.shader");
    const std::string fragmentShaderSource = readFile("simpleFragment.shader");

    const char* vertex_shader_text = vertexShaderSource.c_str();
    const char* fragment_shader_text = fragmentShaderSource.c_str();

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glUseProgram(program);

    const GLint modelLoc = glGetUniformLocation(program, "model");
    const GLint viewLoc = glGetUniformLocation(program, "view");
    const GLint projectionLoc = glGetUniformLocation(program, "projection");

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_identity(view);
    mat4x4_identity(projection);

    mat4x4_translate_in_place(view, 0.0f, 0.0f, -3.0f);

    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
    float aspect = (float)width / (float)height;
    mat4x4_perspective(projection, 1.0f, aspect, 0.1f, 100.0f);

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)&view);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)&projection);

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    int stride = (3 + 3 + 2 + 3) * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    setupTextures();

    // Set light properties
    glUniform3f(glGetUniformLocation(program, "light.position"), 1.2f, 10.0f, 2.0f);
    glUniform3f(glGetUniformLocation(program, "light.ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "light.diffuse"), 3.0f, 3.0f, 3.0f);
    glUniform3f(glGetUniformLocation(program, "light.specular"), 1.0f, 1.0f, 1.0f);

    // Set material properties
    glUniform3f(glGetUniformLocation(program, "material.ambient"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "material.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(program, "material.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(program, "material.shininess"), 32.0f);

    const float rotationSpeed = 1.0f; // Adjust rotation speed

    //voor de rendering
    const GLint timeLoc = glGetUniformLocation(program, "time");

    while (!glfwWindowShouldClose(glfwGetCurrentContext())) {
        int width, height;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
        float aspect = (float)width / (float)height;

        float timeValue = (float)glfwGetTime(); // Tijdvariabele voor animatie
        glUniform1f(timeLoc, timeValue); // Stel de tijd uniform in


        mat4x4_perspective(projection, 1.0f, aspect, 0.1f, 100.0f);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, (const GLfloat*)&projection);

        processInput(glfwGetCurrentContext());

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float angle = (float)glfwGetTime() * rotationSpeed;
        mat4x4_identity(model);
        mat4x4_rotate_Y(model, model, angle);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)&model);

        glUseProgram(program);
        glBindVertexArray(vertex_array);
        glBindTexture(GL_TEXTURE_2D, dirt); // Bind the texture before drawing
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(cubeIndices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glfwDestroyWindow(glfwGetCurrentContext());
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void Triangle::render(GLFWwindow* window)
{
    if (window == NULL) {
        std::cerr << "Window is NULL!" << std::endl;
        return;
    }

    const GLint modelLoc = glGetUniformLocation(programID, "model");
    const GLint timeLoc = glGetUniformLocation(programID, "time");



    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    const float rotationSpeed = 1.0f; // Adjust rotation speed

    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;

        processInput(window);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bereken de rotatiehoek
        float angle = (float)glfwGetTime() * rotationSpeed;
        float timeValue = (float)glfwGetTime(); // Tijdvariabele voor animatie
        std::cout << "test" << timeValue << std::endl;

        mat4x4 model;
        mat4x4_identity(model);
        mat4x4_rotate_Y(model, model, angle);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)&model);
        glUniform1f(timeLoc, timeValue); // Stel de tijd uniform in

        glUseProgram(programID);
        glBindVertexArray(vertex_array);
        glBindTexture(GL_TEXTURE_2D, dirt); // Bind de texture voor het tekenen
        glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(cubeIndices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vertex_array);
}



void Triangle::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
