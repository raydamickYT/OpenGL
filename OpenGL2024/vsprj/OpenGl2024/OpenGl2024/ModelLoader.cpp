#include "ModelLoader.h"
#include "stb_image.h"

void ModelLoader::loadModels(std::vector<Entity>& entities) {
    Entity templeEntity{};

    //templeEntity.model = new Model("models/OBJ/voorbeeld/uv_sphere.obj");
    templeEntity.model = new Model("models/Japanese_Temple_Model/Japanese_Temple.obj");

    templeEntity.position = glm::vec3(10.0f, 0.0f, 0.0f);
    templeEntity.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    templeEntity.scale = glm::vec3(1.0f);

    //Temple doesn't need the uvs to be flipped. Backpack Does.
    //stbi_set_flip_vertically_on_load(true);

    //Entity backPack{};

    //backPack.model = new Model("models/backpack/backpack.obj");
    //backPack.position = glm::vec3(0, 0, 0);
    //backPack.rotation = glm::vec3(0);
    //backPack.scale = glm::vec3(1);

    //std::cout << "Finished Loading Models" << std::endl;

    //entities.push_back(backPack);

    entities.push_back(templeEntity);
}
