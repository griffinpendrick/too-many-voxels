#pragma once

#include <queue>
#include <unordered_map>

#include "utils/common.h"
#include "utils/camera.h"
#include "utils/shader.h"
#include "chunk.h"

#define RENDER_DISTANCE 16
#define CHUNKS_PER_FRAME 2

struct RaycastInfo
{
	RaycastInfo(Chunk* CChunk, Chunk* RChunk, glm::ivec3 PPos, glm::ivec3 BPos)
	{
		CurrentChunk = CChunk;
		RayChunk = RChunk;
		PlacePosition = PPos;
		BreakPosition = BPos;
	}

	Chunk* CurrentChunk;
	Chunk* RayChunk;
	glm::ivec3 PlacePosition;
	glm::ivec3 BreakPosition;
};

struct ChunkHash
{
	size_t operator()(const glm::ivec3& pos) const
    {
		size_t h1 = std::hash<int>()(pos.x);
		size_t h2 = std::hash<int>()(pos.y);
		size_t h3 = std::hash<int>()(pos.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

struct World 
{
    World();

    void Render(Shader &shader);
    void Update(Camera camera);
	void SetBlock(Chunk* chunk, glm::ivec3 BlockIndex, u8 SelectedBlock, bool Mode);
    inline void LoadChunks(s32 ChunkX, s32 ChunkZ);
    inline void UnloadChunks(s32 ChunkX, s32 ChunkZ);

	RaycastInfo Raycast(glm::vec3 Position, glm::vec3 Direction, f32 MaxReach);

	static World* world; // World "Singleton"
	std::queue<glm::ivec3> ChunkQueue; // Update Queue
    std::unordered_map<glm::ivec3, Chunk*, ChunkHash> Chunks; // Chunk Hash Map
};
