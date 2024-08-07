#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <vector>
#include <glm/glm.hpp>

#include "Renderer.h"
#include "TerrainGenerator.h"
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

class ModelLoader {
public:
    void loadModels(std::vector<Entity>& entities);
};

#endif // MODELLOADER_H
