#pragma once

#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "Renderer.h"
#include "CubeCreator.h"
#include "TextureManager.h"
#include "Window.h"

#include "ThreadPool.h"
#include "perlin_noise.hpp"
#include "ActionQueue.h"

struct Entity
{
	Model* model;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

void initialize_world_information(WorldInformation& worldInformation);

void generate_landscape_chunk(const glm::vec2 currentChunkCord, const glm::vec3 position, const int size, float hScale, float xzScale, glm::vec3 offset, int concurrencyLevel = -1);

void InitializePrograms(Renderer& renderer);

void check_visible_planes();
void load_textures();

void load_models(std::vector<Entity>& entities);

const int width = 1280, height = 720;

WorldInformation worldInformation;

GLuint skyBoxVao, skyBoxEbo;
int skyBoxSize, skyBoxIndexSize;

struct vec2compare
{
	bool operator()(const glm::vec2& a, const glm::vec2& b) const
	{
		if (a.x != b.x)
			return a.x < b.x;
		return a.y < b.y;
	}
};

GLuint skyBoxProgram, cubeProgram, terrainProgram, modelProgram;

std::vector<Entity> entities;

std::map<glm::vec2, Plane, vec2compare> activeTerrainChunks;

const int chunkSize = 241;

const int xScale = 5;

int systemThreadsCount;
const int maxViewDistance = 400;

ThreadPool threadPool(std::thread::hardware_concurrency());

Renderer renderer;

Cube cube;

int main()
{
	TextureManager textManager;
	systemThreadsCount = std::thread::hardware_concurrency();

	Window window(width, height, "OpenGLWindow");

	CubeCreator cubeCreator;
	cubeCreator.createCube(skyBoxVao, skyBoxEbo, skyBoxSize, skyBoxIndexSize);
	cubeCreator.createCube(cube.VAO, cube.EBO, cube.size, cube.IndexSize);

	InitializePrograms(renderer); //hier ook de initialization van de renderer

	int count = 0;

	load_models(entities);

	//load_textures();
	textManager.LoadTextures(renderer, terrainProgram, "terrain", cube);
	textManager.LoadTextures(renderer, modelProgram, "model", cube);

	initialize_world_information(worldInformation);

	//Create viewport
	glViewport(0, 0, width, height);

	double deltaTime = 0.0;
	double prevousTick = 0.0;
	double frameRate = 0.0;

	while (!window.shouldClose())
	{
		//input

		//Calculate DeltaTime and FrameRate.
		auto time = glfwGetTime();
		deltaTime = time - prevousTick;
		prevousTick = time;
		frameRate = 1.0 / deltaTime;

		window.processInput(worldInformation);
		//background color set & render
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update Sun Color.
		worldInformation.lightPosition = glm::vec3(glm::cos(time * 0.3f), glm::sin(time * 0.3f), 0.0f);

		//rendering
		renderer.render_skybox(skyBoxProgram, worldInformation, skyBoxVao, skyBoxIndexSize);
		renderer.render_cube(cubeProgram, worldInformation, cube);

		for (auto& entity : entities)
		{
			renderer.render_model(entity.model, modelProgram, worldInformation, entity.position, entity.rotation, entity.scale);
		}

		for (auto& plane : activeTerrainChunks)
		{
			renderer.render_plane(terrainProgram, plane.second, worldInformation);
		}

		check_visible_planes();

		window.swapBuffers();
		window.pollEvents();


		//Clear queued functions
		if (!ActionQueue::shared_instance().IsEmpty())
			ActionQueue::shared_instance().ClearFunctionQueue();
	}

	//Dispose of entity pointers.
	for (auto& entity : entities)
	{
		delete entity.model;
	}

	//Terminate
	threadPool.~ThreadPool();
	glfwTerminate();
	return 0;
}

void check_visible_planes()
{
	const int size = chunkSize - 1;
	auto visibleChunks = static_cast<int>(std::round(maxViewDistance / size));
	const float chunkOffset = size * xScale;

	auto cameraPos = worldInformation.cameraPosition;
	glm::vec2 currentCord = glm::vec2(floor(cameraPos.x / chunkSize), floor(cameraPos.z / chunkSize));

	for (int yOffset = -visibleChunks; yOffset <= visibleChunks; ++yOffset)
	{
		for (int xOffset = -visibleChunks; xOffset <= visibleChunks; ++xOffset)
		{
			glm::vec2 currentChunkCord = glm::vec2((xOffset + currentCord.x), (yOffset + currentCord.y));

			if (activeTerrainChunks.contains(currentChunkCord))
			{
				//Need to deactivate non active ones.
				continue;
			}
			else
			{
				glm::vec3 chunkWorldPos = glm::vec3(currentChunkCord.x * chunkOffset, 0.0f, currentChunkCord.y * chunkOffset);
				activeTerrainChunks.insert({ currentChunkCord, Plane() });

				//Queue it to the threadpool for execution.
				threadPool.enqueue([=]()
					{
						generate_landscape_chunk(currentChunkCord, chunkWorldPos, chunkSize, 400.0f, xScale, chunkWorldPos, 1);
					});
			}
		}
	}
}

void load_models(std::vector<Entity>& entities)
{
	Entity templeEntity{};

	templeEntity.model = new Model("models/Japanese_Temple_Model/Japanese_Temple.obj");

	templeEntity.position = glm::vec3(500, 0, 500);
	templeEntity.rotation = glm::vec3(0, 0.1f, 0.0f);
	templeEntity.scale = glm::vec3(25);

	//Temple doesn't need the uvs to be flipped. Backpack Does.
	stbi_set_flip_vertically_on_load(true);

	/*Entity backPack{};

	backPack.model = new Model("Resources/Models/backpack/backpack.obj");
	backPack.position = glm::vec3(750, 150, 750);
	backPack.rotation = glm::vec3(0);
	backPack.scale = glm::vec3(50);

	std::cout << "Finished Loading Models" << std::endl;*/

	//entities.push_back(backPack);
	entities.push_back(templeEntity);
}

void InitializePrograms(Renderer& renderer)
{
	renderer.Intialize(cubeProgram);
	renderer.createProgram(skyBoxProgram, "shaders/notmine/skyVertexShader.glsl", "shaders/notmine/skyFragmentShader.glsl");
	//renderer.createProgram(terrainProgram, "shaders/notmine/simpleTerrainVertex.glsl", "shaders/notmine/simpleTerrainFragment.glsl");
	renderer.createProgram(modelProgram, "shaders/notmine/modelVertex.glsl", "shaders/notmine/modelFragment.glsl");
}

void initialize_world_information(WorldInformation& worldInformation)
{
	worldInformation = WorldInformation();
	worldInformation.lightPosition = glm::normalize(glm::vec3(-0.5, -0.5f, -0.5f));
	worldInformation.cameraPosition = glm::vec3(100.0f, 100.0f, 100.0f);
	//Alleen wanneer de camera zoomed ect. 0.1 ~= 10cm.
	worldInformation.projection = glm::perspective(glm::radians(60.0f), width / (float)height, 0.1f, 5000.0f);
	//1 Camera Position, camera directie, omhoog directie
	worldInformation.view = glm::lookAt(worldInformation.cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
}


static inline glm::vec3 get_vertex(const std::vector<float>& vertices, const int width, const int x, const int z, const int stride) {
	int index = (z * width + x) * stride;
	if (index + 2 < vertices.size()) {
		return glm::vec3(vertices[index], vertices[index + 1], vertices[index + 2]);
	}
	else {
		// Voeg hier passende foutafhandeling toe (bijvoorbeeld door een exception te gooien of een standaardwaarde terug te geven)
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
}


//This function has been made with the help of chat gpt and adjusted later.
static void calculate_normals(std::vector<float>& vertices, const int stride, const int height, const int width) {
	for (unsigned int i = 0; i < (height - 1) * (width - 1); ++i) {
		int x = i % width;
		int z = i / width;

		glm::vec3 sum(0.0f);

		glm::vec3 center = get_vertex(vertices, width, x, z, stride);

		if (x > 0 && z > 0) {
			glm::vec3 left = get_vertex(vertices, width, x - 1, z, stride);
			glm::vec3 down = get_vertex(vertices, width, x, z - 1, stride);
			sum += glm::normalize(glm::cross(down - center, left - center));
		}
		if (x < width - 1 && z > 0) {
			glm::vec3 right = get_vertex(vertices, width, x + 1, z, stride);
			glm::vec3 down = get_vertex(vertices, width, x, z - 1, stride);
			sum += glm::normalize(glm::cross(right - center, down - center));
		}
		if (x > 0 && z < height - 1) {
			glm::vec3 left = get_vertex(vertices, width, x - 1, z, stride);
			glm::vec3 up = get_vertex(vertices, width, x, z + 1, stride);
			sum += glm::normalize(glm::cross(left - center, up - center));
		}
		if (x < width - 1 && z < height - 1) {
			glm::vec3 right = get_vertex(vertices, width, x + 1, z, stride);
			glm::vec3 up = get_vertex(vertices, width, x, z + 1, stride);
			sum += glm::normalize(glm::cross(up - center, right - center));
		}

		glm::vec3 normal = glm::normalize(sum);

		int vertexIndex = (i)*stride + 3;
		if (vertexIndex + 2 < vertices.size()) {
			vertices[vertexIndex] = normal.x;
			vertices[vertexIndex + 1] = normal.y;
			vertices[vertexIndex + 2] = normal.z;
		}
		else {
			std::cerr << "Error: Attempted to access out-of-range vertex normal at index " << vertexIndex << std::endl;
		}
	}
}

void process_plane(const glm::vec2 currentChunkCord, const glm::vec3 position, const std::vector<unsigned int>& indices, const std::vector<float>& vertices)
{
	const int stride = 8;
	unsigned int VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Plane plane{};

	plane.VAO = VAO;
	plane.EBO = EBO;
	plane.vertices = vertices;
	plane.indices = indices;
	plane.position = position;

	//Replace the place holder placed during the dispatch, with the generated plane.
	activeTerrainChunks.insert_or_assign(currentChunkCord, std::move(plane));
}

void generate_landscape_chunk(const glm::vec2 currentChunkCord, const glm::vec3 position, const int size, float hScale, float xzScale, glm::vec3 offset, int concurrencyLevel) {
	const int stride = 8;
	int count = size * size;

	const int gridSize = 400;
	const int octaves = 8;

	std::vector<float> vertices(count * stride);
	std::vector<unsigned int> indices((size - 1) * (size - 1) * 6);

	// Calculate Batch Size based on concurrency level.
	int threadCount = concurrencyLevel < 1 || concurrencyLevel > systemThreadsCount - 1 ? systemThreadsCount - 1 : concurrencyLevel;
	int batchSize = count / threadCount;
	int batches = count / batchSize;

	std::vector<std::thread> threads;

	// Process Primary Batches.
	for (int i = 0; i < batches; i++) {
		int start = i * batchSize;

		threads.emplace_back([=, &vertices]() {
			int vertexIndex = start * 8;

			for (int loopIndex = start; loopIndex < start + batchSize; ++loopIndex) {
				int x = loopIndex % size;
				int z = loopIndex / size;

				float globalX = x * xzScale;
				float globalZ = z * xzScale;

				if (vertexIndex + 7 < vertices.size()) {
					vertices[vertexIndex++] = globalX;
					float perlinValue = perlin_noise::octaved_perlin_noise(globalX + offset.x, globalZ + offset.z, octaves, gridSize);

					vertices[vertexIndex++] = perlinValue * hScale;
					vertices[vertexIndex++] = globalZ;

					vertices[vertexIndex++] = 0.0f;
					vertices[vertexIndex++] = 0.0f;
					vertices[vertexIndex++] = 0.0f;

					vertices[vertexIndex++] = x / (float)size;
					vertices[vertexIndex++] = z / (float)size;
				}
				else {
					std::cerr << "Error: Attempted to access out-of-range vertex at index " << vertexIndex << std::endl;
				}
			}
			});
	}

	// Process Remainder while other batches are being processed.
	int remaining = count % threadCount;

	if (remaining > 0) {
		int start = count - remaining;
		int vertexIndex = start * 8;

		for (int i = start; i < start + remaining; ++i) {
			int x = i % size;
			int z = i / size;

			float globalX = x * xzScale;
			float globalZ = z * xzScale;

			if (vertexIndex + 7 < vertices.size()) {
				vertices[vertexIndex++] = globalX;
				float perlinValue = perlin_noise::octaved_perlin_noise(globalX + offset.x, globalZ + offset.z, octaves, gridSize);

				vertices[vertexIndex++] = perlinValue * hScale;
				vertices[vertexIndex++] = globalZ;

				vertices[vertexIndex++] = 0.0f;
				vertices[vertexIndex++] = 0.0f;
				vertices[vertexIndex++] = 0.0f;

				vertices[vertexIndex++] = x / (float)size;
				vertices[vertexIndex++] = z / (float)size;
			}
			else {
				std::cerr << "Error: Attempted to access out-of-range vertex at index " << vertexIndex << std::endl;
			}
		}
	}

	for (auto& thread : threads) {
		thread.join();
	}
	threads.clear();

	unsigned int index = 0;
	for (unsigned int i = 0; i < (size - 1) * (size - 1); ++i) {
		int x = i % (size - 1);
		int z = i / (size - 1);

		int vertex = z * size + x;

		if (vertex + size + 1 < vertices.size() / stride) {
			indices[index++] = vertex;
			indices[index++] = vertex + size;
			indices[index++] = vertex + size + 1;
			indices[index++] = vertex;
			indices[index++] = vertex + size + 1;
			indices[index++] = vertex + 1;
		}
		else {
			std::cerr << "Error: Attempted to access out-of-range index at vertex " << vertex << std::endl;
		}
	}

	calculate_normals(vertices, stride, size, size);

	// Defer finalization to the main thread.
	ActionQueue::shared_instance().AddActionToQueue([=, indices = std::move(indices), vertices = std::move(vertices)]() mutable {
		process_plane(currentChunkCord, position, indices, vertices);
		});
}
