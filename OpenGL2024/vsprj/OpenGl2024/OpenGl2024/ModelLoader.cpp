#include "ModelLoader.h"
#include "stb_image.h"

void ModelLoader::loadModels(std::vector<Entity>& entities) {
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
