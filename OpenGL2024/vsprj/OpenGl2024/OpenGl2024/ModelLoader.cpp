#include "ModelLoader.h"
#include "stb_image.h"

void ModelLoader::loadModels(std::vector<Entity>& entities) {
    Entity SwordEntity{};

    SwordEntity.model = new Model("models/OBJ/Sword/sword.obj");

    SwordEntity.position = glm::vec3(10.0f, 300.0f, 0.0f);
    SwordEntity.rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    SwordEntity.scale = glm::vec3(10.0f);

    entities.push_back(SwordEntity);

    //alleen de backpack moet geflipped worden
    //stbi_set_flip_vertically_on_load(true);

    //Entity backPack{};

    //backPack.model = new Model("models/backpack/backpack.obj");
    //backPack.position = glm::vec3(0, 0, 0);
    //backPack.rotation = glm::vec3(0);
    //backPack.scale = glm::vec3(1);

    //std::cout << "Finished Loading Models" << std::endl;

    //entities.push_back(backPack);

}
