#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <vector>

#include "FastNoiseLite.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "utils/common.h"
#include "utils/shader.h"
#include "block.h"

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 128
#define WATER_LEVEL 27

#define NOISE_OCTAVES 4
#define NOISE_SEED 999

typedef struct
{
    glm::vec2 TexCoords;
    glm::vec3 Position;
    glm::vec3 Normal;
} Vertex;

typedef struct
{
    u32 VAO, VBO, EBO;
    glm::ivec3 Position;
    std::vector<u32> Indices;
    std::vector<Vertex> Vertices;
    std::vector<u8> Blocks;
} Chunk;

void UpdateChunk(Chunk* chunk);
void DeleteChunk(Chunk* chunk);
void GenerateChunk(Chunk* chunk);
void GenerateChunkMesh(Chunk* chunk);
void GenerateBlockMesh(Chunk* chunk, const u8 x, const u8 y, const u8 z);
inline void AddFace(Chunk* chunk, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4, glm::vec3& normal, const glm::vec2 uv[]);
inline u8 GetHeightMap(Chunk* chunk, const u8 x, const u8 z);
inline u16 GetBlockIndex(const u8 x, const u8 y, const u8 z);

#endif