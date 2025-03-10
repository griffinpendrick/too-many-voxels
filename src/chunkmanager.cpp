#include "chunkmanager.h"

void SetBlock(Chunk* chunk, glm::ivec3 BlockPosition, u8 CurrentHeldBlock, bool PlaceMode)
{
    u16 Index = GetBlockIndex(BlockPosition.x, BlockPosition.y, BlockPosition.z);
    
	// Either Places or Breaks Block, Then Updates the Chunk Mesh
	PlaceMode ? chunk->Blocks[Index] = CurrentHeldBlock : chunk->Blocks[Index] = BlockType::AIR;
	UpdateChunk(chunk);
}

void UpdateWorld(const Camera camera)
{
	// Converts Cameras World Position into Chunk Coords
    s32 PlayerChunkX = floor_(camera.Position.x / CHUNK_SIZE);
    s32 PlayerChunkZ = floor_(camera.Position.z / CHUNK_SIZE);

    LoadChunks(PlayerChunkX, PlayerChunkZ);
    UnloadChunks(PlayerChunkX, PlayerChunkZ);

	// Updates Queued Chunks, Amount Processed Per Frame Capped by CHUNKS_PER_FRAME
    u8 ChunksProcessed = 0;
    while (!Manager.UpdateQueue.empty() && ChunksProcessed++ != CHUNKS_PER_FRAME)
	{
        glm::ivec3 pos = Manager.UpdateQueue.front();
        Manager.UpdateQueue.pop();

        if (Manager.Chunks.find(pos) != Manager.Chunks.end())
        {
			UpdateChunk(Manager.Chunks[pos]);
        }
    }
}

inline void LoadChunks(const s32 PlayerChunkX, const s32 PlayerChunkZ) 
{
    u8 CurrentRadius = 0;
    while (CurrentRadius <= RENDER_DISTANCE)
    {
        // Forward
		for (s8 i = -CurrentRadius; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(i + PlayerChunkX, 0, CurrentRadius + PlayerChunkZ);
			if (Manager.Chunks.find(pos) == Manager.Chunks.end())
			{
				Chunk* chunk = new Chunk;
				chunk->Position = pos;
				chunk->Blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, BlockType::AIR);
				Manager.Chunks[pos] = chunk;
				GenerateChunk(chunk);
				Manager.UpdateQueue.push(pos);
			}
		}
        // Right 
		for (s8 i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(CurrentRadius + PlayerChunkX, 0, i + PlayerChunkZ);
			if (Manager.Chunks.find(pos) == Manager.Chunks.end())
			{
				Chunk* chunk = new Chunk;
				chunk->Position = pos;
				chunk->Blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, BlockType::AIR);
				Manager.Chunks[pos] = chunk;
				GenerateChunk(chunk);
				Manager.UpdateQueue.push(pos);
			}
		}
        // Backward
		for (s8 i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(i + PlayerChunkX, 0, -CurrentRadius + PlayerChunkZ);
			if (Manager.Chunks.find(pos) == Manager.Chunks.end())
			{
				Chunk* chunk = new Chunk;
				chunk->Position = pos;
				chunk->Blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, BlockType::AIR);
				Manager.Chunks[pos] = chunk;
				GenerateChunk(chunk);
				Manager.UpdateQueue.push(pos);
			}
		}
        // Left
		for (s8 i = -CurrentRadius; i <= CurrentRadius - 1; ++i)
		{
			glm::ivec3 pos(-CurrentRadius + PlayerChunkX, 0, i + PlayerChunkZ);
			if (Manager.Chunks.find(pos) == Manager.Chunks.end())
			{
				Chunk* chunk = new Chunk;
				chunk->Position = pos;
				chunk->Blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE, BlockType::AIR);
				Manager.Chunks[pos] = chunk;
				GenerateChunk(chunk);
				Manager.UpdateQueue.push(pos);
			}
		}
        CurrentRadius++;
    }
}

inline void UnloadChunks(const s32 PlayerChunkX, const s32 PlayerChunkZ) {
	for (auto it = Manager.Chunks.begin(); it != Manager.Chunks.end();)
    {
        s32 ChunkX = it->first.x;
        s32 ChunkZ = it->first.z;

        if (abs_(ChunkX - PlayerChunkX) > RENDER_DISTANCE || abs_(ChunkZ - PlayerChunkZ) > RENDER_DISTANCE)
        {
			DeleteChunk(it->second);
            it = Manager.Chunks.erase(it);
        }
		else
		{
			++it;
		}
    }
}

void RenderWorld(Shader& shader)
{
    for (auto& [key, chunk] : Manager.Chunks)
    {
		// Calculates Chunks World Position
        glm::vec3 WorldPosition(chunk->Position.x * CHUNK_SIZE, chunk->Position.y * CHUNK_HEIGHT, chunk->Position.z * CHUNK_SIZE);

		// Passes Chunks Position in World Space to Shader Before Rendering
        glm::mat4 model = glm::translate(glm::mat4(1.0f), WorldPosition);
        shader.SetMat4("Model", model);

        glBindVertexArray(chunk->VAO);
        glDrawElements(GL_TRIANGLES, chunk->Indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
    } 
}