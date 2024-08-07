#ifndef MAIN_H
#define MAIN_H

#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "glm/gtx/string_cast.hpp"


#include "ModelLoader.h"

class Main 
{
public:
	void initialize_world_information(WorldInformation& worldInformation);

	void SetupPrograms(Renderer& renderer);

	const int width = 1280, height = 720;

	WorldInformation worldInformation;

	GLuint skyBoxVao, skyBoxEbo;
	int skyBoxSize, skyBoxIndexSize;

	GLuint skyBoxProgram, cubeProgram, terrainProgram, modelProgram;

	std::vector<Entity> entities;

	std::map<glm::vec2, Plane, vec2compare> activeTerrainChunks;

	int systemThreadsCount;
	const int maxViewDistance = 400;
	const int chunkSize = 241;
	const int xScale = 5;

		Renderer renderer;

	Cube cube;
};
#endif // !MAIN_H

