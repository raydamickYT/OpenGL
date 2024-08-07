#ifndef CUBE_CREATOR_H
#define CUBE_CREATOR_H

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>

class CubeCreator
{
public:
    CubeCreator();
    void createCube(GLuint& VAO, GLuint& EBO, int& size, int& numIndices);

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

#endif // CUBE_CREATOR_H
