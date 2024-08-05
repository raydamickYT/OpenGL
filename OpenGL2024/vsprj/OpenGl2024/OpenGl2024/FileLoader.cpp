#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "FileLoader.h"

unsigned int FileLoader::load_GL_texture(const char* filePath, int comp)
{
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &numChannels, comp);

    if (data != NULL)
    {
        if (comp != 0) numChannels = comp;

        switch (numChannels)
        {
        case (3):
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        }
        case (4):
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        }
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed To Load Texture." << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

void load_file(const char* filePath, std::promise<char*> promise)
{
    std::ifstream file(filePath, std::ios::binary);
    char* output;

    if (file.is_open())
    {
        file.seekg(0, file.end);
        int lenght = file.tellg();

        file.seekg(0, file.beg);

        output = new char[lenght + 1];
        file.read(output, lenght);

        output[lenght] = '\0';

        file.close();
    }
    else
    {
        output = NULL;
    }
    promise.set_value(output);
}

std::future<char*> FileLoader::load_file_async(const char* filePath)
{
    std::promise<char*> p;
    auto future = p.get_future();

    std::thread thread = std::thread(load_file, filePath, std::move(p));
    thread.detach();

    return future;
}
