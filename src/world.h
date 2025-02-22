#pragma once

#include <vector>
#include <queue>
#include <unordered_map>

#include "FastNoiseLite.h"
#include "utils/camera.h"
#include "utils/shader.h"
#include "chunk.h"

static const int RENDER_DISTANCE = 8;
static const int CHUNKS_PER_FRAME = 1;

struct ChunkHash
{
	size_t operator()(const glm::vec3& pos) const
    {
		std::size_t h1 = std::hash<int>()(pos.x);
		std::size_t h2 = std::hash<int>()(pos.y);
		std::size_t h3 = std::hash<int>()(pos.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

struct World 
{
    World();

	static World* world;

    void Render(Shader &shader);
    void Update(Camera camera);

    inline void LoadChunks(int ChunkX, int ChunkZ);
    inline void UnloadChunks(int ChunkX, int ChunkZ);

	std::queue<glm::ivec3> ChunkQueue;
    std::unordered_map<glm::ivec3, Chunk*, ChunkHash> Chunks;
};
