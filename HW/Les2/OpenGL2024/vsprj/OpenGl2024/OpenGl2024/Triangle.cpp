#include "Triangle.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Triangle::Triangle() {}

Triangle::~Triangle() {}

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

void Triangle::setupShaders() {
    // Shader setup code here
}

GLuint createSquare() {
    GLuint squareProgram;
    static const char* square_vertex_shader_text =
        "#version 330 core\n"
        "layout(location = 0) in vec2 vPos;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(vPos, 0.0, 1.0);\n"
        "}\n";

    static const char* square_fragment_shader_text =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n" // Rood vierkant
        "}\n";

    // Compileer de vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &square_vertex_shader_text, NULL);
    glCompileShader(vertexShader);

    // Compileer de fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &square_fragment_shader_text, NULL);
    glCompileShader(fragmentShader);

    // Link de shaders naar een programma
    squareProgram = glCreateProgram();
    glAttachShader(squareProgram, vertexShader);
    glAttachShader(squareProgram, fragmentShader);
    glLinkProgram(squareProgram);

    // Opruimen
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return squareProgram;
}

GLuint createSquareVAO() {
    GLuint squareVAO, squareVBO;
    static const float squareVertices[] = {
        -0.5f, -0.5f, // Linksonder
         0.5f, -0.5f, // Rechtsonder
         0.5f,  0.5f, // Rechtsboven
        -0.5f,  0.5f  // Linksboven
    };

    // Maak en configureer een VAO en VBO voor het vierkant
    glGenVertexArrays(1, &squareVAO);
    glGenBuffers(1, &squareVBO);

    glBindVertexArray(squareVAO);

    glBindBuffer(GL_ARRAY_BUFFER, squareVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Opruimen
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return squareVAO;
}

void renderSquare(GLuint squareProgram, GLuint squareVAO) {
    glUseProgram(squareProgram);
    glBindVertexArray(squareVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    glUseProgram(0);
}
