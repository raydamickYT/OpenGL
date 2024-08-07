#include "TerrainGenerator.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "ActionQueue.h"
#include "perlin_noise.hpp"

void TerrainGenerator::initialize(ThreadPool& pool) {
    threadPool = &pool;
    systemThreadsCount = std::thread::hardware_concurrency();
}

void TerrainGenerator::generateLandscapeChunk(const glm::vec2 currentChunkCord, const glm::vec3 position, const int size, float hScale, float xzScale, glm::vec3 offset, int concurrencyLevel) {
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

    calculateNormals(vertices, stride, size, size);

    // Defer finalization to the main thread.
    ActionQueue::shared_instance().AddActionToQueue([=, indices = std::move(indices), vertices = std::move(vertices)]() mutable {
        processPlane(currentChunkCord, position, indices, vertices);
        });
}

void TerrainGenerator::checkVisiblePlanes(WorldInformation& worldInfo, std::map<glm::vec2, Plane, vec2compare>& activeTerrainChunks)
{
    activeTerrainChunks2 = &activeTerrainChunks;
    const int size = chunkSize - 1;
    auto visibleChunks = static_cast<int>(std::round(maxViewDistance / size));
    const float chunkOffset = size * xScale;

    auto cameraPos = worldInfo.cameraPosition;
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
                threadPool->enqueue([=]()
                    {
                        generateLandscapeChunk(currentChunkCord, chunkWorldPos, chunkSize, 400.0f, xScale, chunkWorldPos, 1);
                    });
            }
        }
    }
    //terrainGenerator.checkVisiblePlanes(worldInformation.cameraPosition, activeTerrainChunks, worldInformation);
}

void TerrainGenerator::processPlane(const glm::vec2 currentChunkCord, const glm::vec3 position, const std::vector<unsigned int>& indices, const std::vector<float>& vertices) {
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

    activeTerrainChunks2->insert_or_assign(currentChunkCord, std::move(plane));
}

glm::vec3 TerrainGenerator::getVertex(const std::vector<float>& vertices, const int width, const int x, const int z, const int stride) {
    int index = (z * width + x) * stride;
    if (index + 2 < vertices.size()) {
        return glm::vec3(vertices[index], vertices[index + 1], vertices[index + 2]);
    }
    else {
        // Voeg hier passende foutafhandeling toe (bijvoorbeeld door een exception te gooien of een standaardwaarde terug te geven)
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}

void TerrainGenerator::calculateNormals(std::vector<float>& vertices, const int stride, const int height, const int width) {
    for (unsigned int i = 0; i < (height - 1) * (width - 1); ++i) {
        int x = i % width;
        int z = i / width;

        glm::vec3 sum(0.0f);

        glm::vec3 center = getVertex(vertices, width, x, z, stride);

        if (x > 0 && z > 0) {
            glm::vec3 left = getVertex(vertices, width, x - 1, z, stride);
            glm::vec3 down = getVertex(vertices, width, x, z - 1, stride);
            sum += glm::normalize(glm::cross(down - center, left - center));
        }
        if (x < width - 1 && z > 0) {
            glm::vec3 right = getVertex(vertices, width, x + 1, z, stride);
            glm::vec3 down = getVertex(vertices, width, x, z - 1, stride);
            sum += glm::normalize(glm::cross(right - center, down - center));
        }
        if (x > 0 && z < height - 1) {
            glm::vec3 left = getVertex(vertices, width, x - 1, z, stride);
            glm::vec3 up = getVertex(vertices, width, x, z + 1, stride);
            sum += glm::normalize(glm::cross(left - center, up - center));
        }
        if (x < width - 1 && z < height - 1) {
            glm::vec3 right = getVertex(vertices, width, x + 1, z, stride);
            glm::vec3 up = getVertex(vertices, width, x, z + 1, stride);
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
