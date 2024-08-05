#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include "FileLoader.h"
#include "Model.h"

// Forward declaration
class Renderer;

struct WorldInformation
{
	glm::vec3 cameraPosition = glm::vec3();
	glm::mat4 projection = glm::mat4();
	glm::mat4 view = glm::mat4();
	glm::vec3 lightPosition = glm::vec3();
};

struct Plane
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	unsigned int VAO;
	unsigned int EBO;
	glm::vec3 position;
	std::vector<unsigned int> textures;

	void validate(Renderer* renderer);
};

struct Cube
{
	glm::vec3 Position;
	glm::vec3 Scale;
	glm::vec3 Rotation;
	std::vector<unsigned int> Textures;
	int size;
	unsigned int VAO;
	unsigned int EBO;
	int IndexSize;

	void validate(Renderer* renderer);
};

class Renderer
{
public:
	void Intialize(GLuint& program);
	void render_plane(unsigned int& planeProgram, Plane& plane, WorldInformation& worldInformation);
	void render_cube(unsigned int& cubeProgram, WorldInformation& worldInformation, Cube& cube);
	void render_skybox(unsigned int& skyProgram, WorldInformation& worldInformation, unsigned int skyboxVao, unsigned int skyBoxIndexSize);
	void createProgram(GLuint& programId, const char* vertex, const char* fragment);
	void render_model(Model* model, unsigned int program, WorldInformation worldInformation, glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale);
	void process_uniforms(unsigned int& program, WorldInformation& worldInformation, glm::mat4& worldMatrix);
	unsigned int generateDefaultTexture();

	const glm::vec3 topColor = glm::vec3(68.0 / 255.0, 118.0 / 255.0, 189.0 / 255.0);
	const glm::vec3 botColor = glm::vec3(188.0 / 255.0, 214.0 / 255.0, 231.0 / 255.0);
	glm::vec3 sunColor = glm::vec3(1.0, 200.0 / 255.0, 50.0 / 255.0);

	unsigned int dirt, sand, grass, rock, snow;

private:
	std::string loadShaderSource(const char* filePath);
	unsigned int compileShaderSource(const char* source, GLenum shaderType);
};
