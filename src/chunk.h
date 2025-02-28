#pragma once

#include <vector>

#include "FastNoiseLite.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "utils/common.h"
#include "utils/shader.h"
#include "block.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define WATER_LEVEL 27

#define NOISE_OCTAVES 4
#define NOISE_SEED 999

inline u16 GetBlockIndex(u8 x, u8 y, u8 z);

struct Vertex
{
    glm::vec2 TexCoords;
    glm::vec3 Position;
    glm::vec3 Normal;
};

struct Chunk
{
    Chunk(glm::ivec3 ChunkPosition);
    ~Chunk();

    void UpdateChunk();
    void GenerateChunk();
    void GenerateChunkMesh();
    void GenerateBlockMesh(u8 x, u8 y, u8 z);
    inline void GenerateTreeMesh(u8 x, u8 y, u8 z);

    inline u8 GetHeightMap(Chunk* chunk, u8 x, u8 z);
    inline void AddFace(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4, glm::vec3& normal, const glm::vec2 uv[]);

    bool Generated;
    u32 VAO, VBO, EBO;

    u8 Blocks[CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE];
    glm::ivec3 Position;

    std::vector<u32> indices;
    std::vector<Vertex> vertices;
};