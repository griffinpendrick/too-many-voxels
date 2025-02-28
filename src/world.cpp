#include "world.h"

World* World::world = nullptr;

World::World()
{
    World::world = this;
}

void World::Update(Camera camera)
{
	// Converts Cameras World Position into Chunk Coords
    s32 PlayerChunkX = floor_(camera.Position.x / CHUNK_SIZE);
    s32 PlayerChunkZ = floor_(camera.Position.z / CHUNK_SIZE);

    LoadChunks(PlayerChunkX, PlayerChunkZ);
    UnloadChunks(PlayerChunkX, PlayerChunkZ);

	// Updates Queued Chunks, Amount Processed Per Frame Capped by CHUNKS_PER_FRAME
    u8 ChunksProcessed = 0;
    while (!ChunkQueue.empty() && ChunksProcessed++ != CHUNKS_PER_FRAME)
	{
        glm::ivec3 pos = ChunkQueue.front();
        ChunkQueue.pop();

        if (Chunks.find(pos) != Chunks.end())
        {
			Chunks[pos]->UpdateChunk();
        }
    }
}

inline void World::LoadChunks(s32 PlayerChunkX, s32 PlayerChunkZ) 
{
    u8 CurrentRadius = 0;
    while (CurrentRadius <= RENDER_DISTANCE)
    {
        // Forward
		for (s8 i = -CurrentRadius; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(i + PlayerChunkX, 0, CurrentRadius + PlayerChunkZ);
			if (Chunks.find(pos) == Chunks.end())
			{
				Chunks[pos] = new Chunk(pos);
				Chunks[pos]->GenerateChunk();
				ChunkQueue.push(pos);
			}
		}
        // Right 
		for (s8 i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(CurrentRadius + PlayerChunkX, 0, i + PlayerChunkZ);
			if (Chunks.find(pos) == Chunks.end())
			{
				Chunks[pos] = new Chunk(pos);
				Chunks[pos]->GenerateChunk();
				ChunkQueue.push(pos);
			}
		}
        // Backward
		for (s8 i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
		{
			glm::ivec3 pos(i + PlayerChunkX, 0, -CurrentRadius + PlayerChunkZ);
			if (Chunks.find(pos) == Chunks.end())
			{
				Chunks[pos] = new Chunk(pos);
				Chunks[pos]->GenerateChunk();
				ChunkQueue.push(pos);
			}
		}
        // Left
		for (s8 i = -CurrentRadius; i <= CurrentRadius - 1; ++i)
		{
			glm::ivec3 pos(-CurrentRadius + PlayerChunkX, 0, i + PlayerChunkZ);
			if (Chunks.find(pos) == Chunks.end())
			{
				Chunks[pos] = new Chunk(pos);
				Chunks[pos]->GenerateChunk();
				ChunkQueue.push(pos);
			}
		}
        CurrentRadius++;
    }
}

inline void World::UnloadChunks(s32 PlayerChunkX, s32 PlayerChunkZ) {
	for (auto it = Chunks.begin(); it != Chunks.end();)
    {
        s32 ChunkX = it->first.x;
        s32 ChunkZ = it->first.z;

        if (abs_(ChunkX - PlayerChunkX) > RENDER_DISTANCE || abs_(ChunkZ - PlayerChunkZ) > RENDER_DISTANCE)
        {
            delete it->second;
            it = Chunks.erase(it);
        }
		else
		{
			++it;
		}
    }
}

void World::Render(Shader& shader)
{
    for (auto& [key, chunk] : Chunks)
    {
		// Calculates Chunks World Position
        glm::vec3 WorldPosition(chunk->Position.x * CHUNK_SIZE, chunk->Position.y * CHUNK_HEIGHT, chunk->Position.z * CHUNK_SIZE);

		// Passes Chunks Position in World Space to Shader Before Rendering
        glm::mat4 model = glm::translate(glm::mat4(1.0f), WorldPosition);
        shader.SetMat4("Model", model);

        glBindVertexArray(chunk->VAO);
        glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
    }
}

RaycastInfo World::Raycast(glm::vec3 Position, glm::vec3 Direction, f32 MaxReach)
{
    f32 CurrentReach = 0.0f;
    glm::vec3 LastEmptyBlock = Position;

    while (CurrentReach < MaxReach)
    {
        CurrentReach += 0.01f;
        glm::vec3 Result = Position + Direction * CurrentReach;

		// Convert Result to Chunk Space
		s32 ChunkX     = floor_(Result.x / CHUNK_SIZE);
		s32 ChunkZ     = floor_(Result.z / CHUNK_SIZE);
		s32 LastChunkX = floor_(LastEmptyBlock.x / CHUNK_SIZE);
		s32 LastChunkZ = floor_(LastEmptyBlock.z / CHUNK_SIZE);

		// Check if Valid Chunks
		if (Chunks.find({ChunkX, 0, ChunkZ}) == Chunks.end() || Chunks.find({LastChunkX, 0, LastChunkZ}) == Chunks.end())
		{
			LastEmptyBlock = Result;
			continue;
		}

		Chunk* LocalBlockChunk = World::world->Chunks[{ChunkX, 0, ChunkZ}];
		Chunk* LastEmptyBlockChunk = World::world->Chunks[{LastChunkX, 0, LastChunkZ}];

		s32 LocalX = floor_(Result.x + BLOCK_RENDER_SIZE) - (ChunkX * CHUNK_SIZE);
		s32 LocalY = floor_(Result.y + BLOCK_RENDER_SIZE);
		s32 LocalZ = floor_(Result.z + BLOCK_RENDER_SIZE) - (ChunkZ * CHUNK_SIZE);
		s32 LastEmptyLocalX = floor_(LastEmptyBlock.x + BLOCK_RENDER_SIZE) - (LastChunkX * CHUNK_SIZE);
		s32 LastEmptyLocalY = floor_(LastEmptyBlock.y + BLOCK_RENDER_SIZE);
		s32 LastEmptyLocalZ = floor_(LastEmptyBlock.z + BLOCK_RENDER_SIZE) - (LastChunkZ * CHUNK_SIZE);

		clamp_(LocalX, 0, CHUNK_SIZE - 1);
		clamp_(LocalY, 0, CHUNK_HEIGHT - 1);
		clamp_(LocalZ, 0, CHUNK_SIZE - 1);
		clamp_(LastEmptyLocalX, 0, CHUNK_SIZE - 1);
		clamp_(LastEmptyLocalY, 0, CHUNK_HEIGHT - 1);
		clamp_(LastEmptyLocalZ, 0, CHUNK_SIZE - 1);

		// Enforce Height Limits 
		if ((LastEmptyBlock.y <= 1 || LastEmptyBlock.y >= CHUNK_HEIGHT) || !LocalBlockChunk->Blocks[GetBlockIndex(LocalX, LocalY, LocalZ)])
		{
			LastEmptyBlock = Result;
			continue;
		}

		return RaycastInfo(LocalBlockChunk, LastEmptyBlockChunk, glm::ivec3(LastEmptyLocalX, LastEmptyLocalY, LastEmptyLocalZ), glm::ivec3(LocalX, LocalY, LocalZ));
    }

    return RaycastInfo(nullptr, nullptr, glm::ivec3(0), glm::ivec3(0)); // Return NULL hit
}

void World::SetBlock(Chunk* chunk, glm::ivec3 BlockPosition, u8 SelectedBlock, bool PlaceMode)
{
    u16 Index = GetBlockIndex(BlockPosition.x, BlockPosition.y, BlockPosition.z);
    
	// Either Places or Breaks Block, Then Updates the Chunk Mesh
	PlaceMode ? chunk->Blocks[Index] = SelectedBlock : chunk->Blocks[Index] = BlockType::AIR;
	chunk->UpdateChunk();
}