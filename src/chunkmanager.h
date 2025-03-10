#ifndef __CHUNKMANAGER_H__
#define __CHUNKMANAGER_H__

#include <queue>
#include <unordered_map>

#include "utils/common.h"
#include "utils/camera.h"
#include "utils/shader.h"
#include "chunk.h"

#define RENDER_DISTANCE 16
#define CHUNKS_PER_FRAME 1 

typedef struct
{
	size_t operator()(const glm::ivec3& pos) const
    {
		size_t h1 = std::hash<int>()(pos.x);
		size_t h2 = std::hash<int>()(pos.y);
		size_t h3 = std::hash<int>()(pos.z);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
} ChunkHash;

typedef struct 
{
	std::queue<glm::ivec3> UpdateQueue;
	std::unordered_map<glm::ivec3, Chunk*, ChunkHash> Chunks;
} ChunkManager;

inline ChunkManager Manager; // Global Chunk Manager

void RenderWorld(Shader& shader);
void UpdateWorld(const Camera camera);
void SetBlock(Chunk* chunk, glm::ivec3 BlockIndex, u8 CurrentHeldBlock, bool Mode);
inline void LoadChunks(const s32 PlayerChunkX, const s32 PlayerChunkZ);
inline void UnloadChunks(const s32 PlayerChunkX, const s32 PlayerChunkZ);

#endif
