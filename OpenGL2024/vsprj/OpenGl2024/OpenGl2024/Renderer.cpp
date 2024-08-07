#include "Renderer.h"

void Plane::validate(Renderer* renderer) {
    std::cout << "Validating Plane..." << std::endl;
    std::cout << "Vertices size: " << vertices.size() << std::endl;
    std::cout << "Indices size: " << indices.size() << std::endl;

    if (vertices.empty()) {
        //throw std::runtime_error("Plane vertices vector is empty!");
        std::cerr << "vertices are empty" << std::endl;
    }
    if (indices.empty()) {
        std::cerr << "plane indices are empty" << std::endl;
        //throw std::runtime_error("Plane indices vector is empty!");
    }
}

void Cube::validate(Renderer* renderer) {
    if (Textures.empty()) {
        std::cout << "Cube textures vector is empty! Adding default texture." << std::endl;
        Textures.push_back(renderer->generateDefaultTexture());
    }
}

unsigned int Renderer::generateDefaultTexture()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    // Stel de standaardkleur in (knalroze)
    unsigned char color[3] = { 255, 0, 255 }; // RGB waarden voor knalroze

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void Renderer::Intialize(GLuint& program)
{
    createProgram(program, "shaders/simpleVertexShader.glsl", "shaders/simpleFragmentShader.glsl");

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "mainTex"), 0);
    glUniform1i(glGetUniformLocation(program, "normalTex"), 1);
}

std::string Renderer::loadShaderSource(const char* filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Renderer::compileShaderSource(const char* source, GLenum shaderType)
{
    unsigned int shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    int success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

void Renderer::createProgram(GLuint& programId, const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode = loadShaderSource(vertexPath);
    if (vertexCode.empty()) return;
    std::string fragmentCode = loadShaderSource(fragmentPath);
    if (fragmentCode.empty()) return;

    unsigned int vertexShader = compileShaderSource(vertexCode.c_str(), GL_VERTEX_SHADER);
    if (!vertexShader) return;
    unsigned int fragmentShader = compileShaderSource(fragmentCode.c_str(), GL_FRAGMENT_SHADER);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return;
    }

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    int success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(programId);
        programId = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::render_cube(unsigned int& cubeProgram, WorldInformation& worldInformation, Cube& cube)
{
    cube.validate(this);

    // Debug informatie om te controleren of texturen correct zijn geladen
    std::cout << "Rendering cube with " << cube.Textures.size() << " textures." << std::endl;

    for (size_t i = 0; i < cube.Textures.size(); ++i) {
        std::cout << "Texture " << i << " ID: " << cube.Textures[i] << std::endl;
    }

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    glUseProgram(cubeProgram);

    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, glm::vec3(0, 300, 10));
    world = world * glm::mat4_cast(glm::quat(glm::vec3(0, 0.5f, 0)));
    world = glm::scale(world, glm::vec3(1));

    process_uniforms(cubeProgram, worldInformation, world);

    for (size_t i = 0; i < cube.Textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, cube.Textures[i]);
    }

    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, cube.IndexSize, GL_UNSIGNED_INT, 0);

    glUseProgram(0);
}

void Renderer::render_skybox(unsigned int& skyProgram, WorldInformation& worldInformation, unsigned int skyboxVao, unsigned int skyBoxIndexSize)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(skyProgram);

    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, worldInformation.cameraPosition);
    world = glm::scale(world, glm::vec3(100.0f, 100.0f, 100.0f));

    process_uniforms(skyProgram, worldInformation, world);

    glBindVertexArray(skyboxVao);
    glDrawElements(GL_TRIANGLES, skyBoxIndexSize, GL_UNSIGNED_INT, 0);

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glUseProgram(0);
}

void Renderer::render_model(Model* model, unsigned int program, WorldInformation worldInformation, glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale) {
    std::cout << "Rendering model with program ID: " << program << std::endl;
    std::cout << "Position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
    std::cout << "camerapos: " << worldInformation.cameraPosition.x << ", " << worldInformation.cameraPosition.y << ", " << worldInformation.cameraPosition.z << std::endl;
    std::cout << "Rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << std::endl;
    std::cout << "Scale: " << scale.x << ", " << scale.y << ", " << scale.z << std::endl;
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //additive
    //glBlendFunc(GL_ONE, GL_ONE);
    //multiply
    //glBlendFunc(GL_DST_COLOR, GL_ZERO);
    //double multiply
    //glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(program);

    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, pos);
    world = world * glm::mat4_cast(glm::quat(rotation));
    world = glm::scale(world, scale);

    process_uniforms(program, worldInformation, world);

    //// Bind de diffuse texture
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, model->GetDiffuseTexture());

    //// Bind de specular texture
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, model->GetSpecularTexture());

    model->Draw(program);

    glUseProgram(0); // Unbind the program
    //glDisable(GL_BLEND);
}

void Renderer::process_uniforms(unsigned int& program, WorldInformation& worldInformation, glm::mat4& worldMatrix)
{
    glUniformMatrix4fv(glGetUniformLocation(program, "world"), 1, GL_FALSE, glm::value_ptr(worldMatrix));
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(worldInformation.projection));
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(worldInformation.view));

    glUniform3fv(glGetUniformLocation(program, "sunColor"), 1, glm::value_ptr(sunColor));
    glUniform3fv(glGetUniformLocation(program, "topColor"), 1, glm::value_ptr(topColor));
    glUniform3fv(glGetUniformLocation(program, "botColor"), 1, glm::value_ptr(botColor));

    //model
    glUniform3fv(glGetUniformLocation(program, "lightDirection"), 1, glm::value_ptr(worldInformation.lightPosition));
    glUniform3fv(glGetUniformLocation(program, "cameraPosition"), 1, glm::value_ptr(worldInformation.cameraPosition));

    // Zorg ervoor dat je de texture units correct gebruikt voor diffuse en specular maps
    glUniform1i(glGetUniformLocation(program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(program, "material.specular"), 1);
}

void Renderer::render_plane(unsigned int& planeProgram, Plane& plane, WorldInformation& worldInformation)
{
    // Debug informatie toevoegen
    std::cout << "Preparing to render plane..." << std::endl;
    std::cout << "Vertices size before validation: " << plane.vertices.size() << std::endl;
    std::cout << "Indices size before validation: " << plane.indices.size() << std::endl;

    plane.validate(this);

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glUseProgram(planeProgram);

    glm::mat4 world = glm::mat4(1.0f);
    world = glm::translate(world, plane.position);

    process_uniforms(planeProgram, worldInformation, world);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dirt);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sand);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grass);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, rock);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, snow);

    glBindVertexArray(plane.VAO);
    glDrawElements(GL_TRIANGLES, plane.indices.size(), GL_UNSIGNED_INT, 0);
    glUseProgram(0);
}
