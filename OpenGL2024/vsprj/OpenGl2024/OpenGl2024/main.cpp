#pragma once
#include "Main.h"

ThreadPool threadPool(std::thread::hardware_concurrency());
Main mn;

int main()
{
	mn.systemThreadsCount = std::thread::hardware_concurrency();
	TextureManager textManager;
	TerrainGenerator::getInstance().initialize(threadPool);

	Window window(mn.width, mn.height, "OpenGLWindow");

	//textures
	textManager.LoadTextures(mn.renderer, mn.terrainProgram, "terrain", mn.cube);
	textManager.LoadTextures(mn.renderer, mn.modelProgram, "model", mn.cube);

	//test cube
	CubeCreator cubeCreator;
	cubeCreator.createCube(mn.skyBoxVao, mn.skyBoxEbo, mn.skyBoxSize, mn.skyBoxIndexSize);
	cubeCreator.createCube(mn.cube.VAO, mn.cube.EBO, mn.cube.size, mn.cube.IndexSize);

	//setup programs
	mn.SetupPrograms(mn.renderer); //hier ook de initialization van de renderer

	//models
	ModelLoader modelLoader;
	modelLoader.loadModels(mn.entities);


	mn.initialize_world_information(mn.worldInformation);

	//Create viewport
	glViewport(0, 0, mn.width, mn.height);

	//voor meer accurate input
	double deltaTime = 0.0;
	double prevousTick = 0.0;
	double frameRate = 0.0;

	while (!window.shouldClose())
	{

		//Calculate DeltaTime and FrameRate.
		auto time = glfwGetTime();
		deltaTime = time - prevousTick;
		prevousTick = time;
		frameRate = 1.0 / deltaTime;

		//input
		window.processInput(mn.worldInformation);
		//background color set & render
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Update Sun Color.
		mn.worldInformation.lightPosition = glm::vec3(glm::cos(time * 0.3f), glm::sin(time * 0.3f), 0.0f);

		//rendering
		mn.renderer.render_skybox(mn.skyBoxProgram, mn.worldInformation, mn.skyBoxVao, mn.skyBoxIndexSize);
		mn.renderer.render_cube(mn.cubeProgram, mn.worldInformation, mn.cube);

		for (auto& entity : mn.entities)
		{
			mn.renderer.render_model(entity.model, mn.modelProgram, mn.worldInformation, entity.position, entity.rotation, entity.scale);
		}

		for (auto& plane : mn.activeTerrainChunks)
		{
			mn.renderer.render_plane(mn.terrainProgram, plane.second, mn.worldInformation);
		}

		//check_visible_planes();
		TerrainGenerator::getInstance().checkVisiblePlanes(mn.worldInformation, mn.activeTerrainChunks);

		window.swapBuffers();
		window.pollEvents();
		

		//Clear queued functions
		if (!ActionQueue::shared_instance().IsEmpty())
			ActionQueue::shared_instance().ClearFunctionQueue();
	}

	//Dispose of entity pointers.
	for (auto& entity : mn.entities)
	{
		delete entity.model;
	}

	//Terminate
	threadPool.~ThreadPool();
	glfwTerminate();
	return 0;
}

void Main::SetupPrograms(Renderer& renderer)
{
	renderer.Intialize(mn.cubeProgram);
	renderer.createProgram(mn.skyBoxProgram, "shaders/skyVertexShader.glsl", "shaders/skyFragmentShader.glsl");
	renderer.createProgram(mn.terrainProgram, "shaders/simpleTerrainVertex.glsl", "shaders/simpleTerrainFragment.glsl");
	renderer.createProgram(mn.modelProgram, "shaders/model.vs", "shaders/model.fragment");
}

void Main::initialize_world_information(WorldInformation& worldInformation)
{
	worldInformation = WorldInformation();
	worldInformation.lightPosition = glm::normalize(glm::vec3(-0.5, -0.5f, -0.5f));
	worldInformation.cameraPosition = glm::vec3(0.0f, 300.0f, 0.0f);

	worldInformation.projection = glm::perspective(glm::radians(60.0f), mn.width / (float)mn.height, 0.1f, 5000.0f);
	
	worldInformation.view = glm::lookAt(worldInformation.cameraPosition, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
}
