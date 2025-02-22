#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "utils/shader.h"

enum BlockType
{
    AIR    = 0,
    GRASS  = 1,
    STONE  = 2,
    SNOW   = 3,
    SAND   = 4,
    WOOD   = 5,
    LEAVES = 6,
    WATER  = 7,
};

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

static const int WATER_LEVEL = 27;
static const int CHUNK_SIZE = 32;
static const int CHUNK_HEIGHT = 150;

static const float TEXTURE_DIMENSION = 16.0f / 64.0f;
static const float BLOCK_RENDER_SIZE = 0.5f;

struct Chunk
{
    Chunk(glm::ivec3 pos);

    void GenerateChunk();
    void GenerateChunkMesh();
    void GenerateBlockMesh(int x, int y, int z);
    inline void GenerateTreeMesh(int x, int y, int z);
    void UpdateChunk();

    inline float GetHeightMap(Chunk* chunk, int x, int z);

    inline void AddFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4,
				const glm::vec3& normal,
				const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4);

    inline int GetBlockIndex(int x, int y, int z);

    bool Generated;

    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<GLint> indices;

    glm::ivec3 Position;
    std::vector<uint8_t> Blocks;
};