#include "TextureManager.h"

void TextureManager::LoadTextures(Renderer& renderer, GLuint program, std::string nameProgram, Cube& cube)
{
	//Textures for the terrain.
	renderer.dirt = FileLoader::load_GL_texture("Textures/dirt.jpg");
	renderer.sand = FileLoader::load_GL_texture("Textures/sand.jpg");
	renderer.grass = FileLoader::load_GL_texture("Textures/grass.png", 4);
	renderer.rock = FileLoader::load_GL_texture("Textures/rock.jpg");
	renderer.snow = FileLoader::load_GL_texture("Textures/snow.jpg");

	if (program != NULL && nameProgram == "terrain")
	{
		glUseProgram(program);

		glUniform1i(glGetUniformLocation(program, "dirt"), 0);
		glUniform1i(glGetUniformLocation(program, "sand"), 1);
		glUniform1i(glGetUniformLocation(program, "grass"), 2);
		glUniform1i(glGetUniformLocation(program, "rock"), 3);
		glUniform1i(glGetUniformLocation(program, "snow"), 4);
	}

	if (program != NULL && nameProgram == "model")
	{
		//Texture setup for the models.
		glUseProgram(program);

		glUniform1i(glGetUniformLocation(program, "texture_diffuse1"), 0);
		glUniform1i(glGetUniformLocation(program, "texture_specular1"), 1);
		glUniform1i(glGetUniformLocation(program, "texture_normal1"), 2);
		glUniform1i(glGetUniformLocation(program, "texture_roughness1"), 3);
		glUniform1i(glGetUniformLocation(program, "texture_ao1"), 4);
	}

	//Textures for the Box.
	auto cubeDiffuse = FileLoader::load_GL_texture("Textures/container2.png");
	auto cubeNormal = FileLoader::load_GL_texture("Textures/container2_normal.png");

	cube.Textures.push_back(cubeDiffuse);
	cube.Textures.push_back(cubeNormal);
}