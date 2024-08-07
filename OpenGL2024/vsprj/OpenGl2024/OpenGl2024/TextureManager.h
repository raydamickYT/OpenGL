#ifndef TEXTURE_MANAGER
#define TEXTURE_MANAGER

#include "Renderer.h"

class TextureManager 
{

public:
	void LoadTextures(Renderer& renderer, GLuint program, std::string nameProgram, Cube& cube);
};
#endif // !TEXTURE_MANAGER

