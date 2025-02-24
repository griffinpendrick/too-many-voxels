#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "utils/shader.h"
#include "block.h"

static const int CHUNK_SIZE = 16;
static const int CHUNK_HEIGHT = 128;
static const int WATER_LEVEL = 27;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Chunk
{
    Chunk(glm::ivec3 pos);
    ~Chunk();

    void GenerateChunk();
    void GenerateChunkMesh();
    void GenerateBlockMesh(int x, int y, int z);
    inline void GenerateTreeMesh(int x, int y, int z);
    void UpdateChunk();

    inline float GetHeightMap(Chunk* chunk, int x, int z);

    inline void AddFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4,
				const glm::vec3& normal,
				const glm::vec2 uv[]);

    inline int GetBlockIndex(int x, int y, int z);

    bool Generated;

    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    glm::ivec3 Position;
    std::vector<uint8_t> Blocks;
};