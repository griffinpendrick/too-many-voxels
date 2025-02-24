#include "world.h"

World* World::world = nullptr;

World::World()
{
    World::world = this;
}

World::~World()
{
}

void World::Update(Camera camera)
{
    int PlayerChunkX = (int)((camera.Position.x) / CHUNK_SIZE);
    int PlayerChunkZ = (int)((camera.Position.z) / CHUNK_SIZE);

    LoadChunks(PlayerChunkX, PlayerChunkZ);
    UnloadChunks(PlayerChunkX, PlayerChunkZ);

    int ChunksProcessed = 0;
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

inline void World::LoadChunks(int PlayerChunkX, int PlayerChunkZ) 
{
    int CurrentRadius = 0;
    while (CurrentRadius <= RENDER_DISTANCE)
    {
        // Forward
		for (int i = -CurrentRadius; i <= CurrentRadius; ++i)
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
		for (int i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
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
		for (int i = -CurrentRadius + 1; i <= CurrentRadius; ++i)
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
		for (int i = -CurrentRadius; i <= CurrentRadius - 1; ++i)
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

inline void World::UnloadChunks(int PlayerChunkX, int PlayerChunkZ) {
	for (auto it = Chunks.begin(); it != Chunks.end(); ++it)
    {
        int ChunkX = it->first.x;
        int ChunkZ = it->first.z;

        if (abs(ChunkX - PlayerChunkX) > RENDER_DISTANCE || abs(ChunkZ - PlayerChunkZ) > RENDER_DISTANCE)
        {
            delete it->second;
            it = Chunks.erase(it);
        } 
    }
}

void World::Render(Shader& shader)
{
    for (auto& [key, chunk] : Chunks)
    {
        glm::vec3 WorldPosition(chunk->Position.x * CHUNK_SIZE, chunk->Position.y * CHUNK_HEIGHT, chunk->Position.z * CHUNK_SIZE);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), WorldPosition);

        shader.SetMat4("model", model);

        glBindVertexArray(chunk->VAO);
        glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void World::Raycast(glm::vec3 Position, glm::vec3 Direction, float MaxReach, int CurrentBlock, bool Mode)
{
	float currentReach = 0.0f;
	glm::ivec3 lastEmptyBlock;
	bool hasEmptyBlock = false;

	while (currentReach < MaxReach)
	{
		currentReach += 0.01f;
		glm::vec3 result = Position + Direction * currentReach;
		glm::ivec3 currentBlock;
		currentBlock.x = std::floor(result.x);
		currentBlock.y = std::floor(result.y);
		currentBlock.z = std::floor(result.z);

		int chunkX = std::floor((float)currentBlock.x / CHUNK_SIZE);
		int chunkZ = std::floor((float)currentBlock.z / CHUNK_SIZE);
		Chunk* chunk = World::world->Chunks[{chunkX, 0, chunkZ}];
		if (chunk)
		{
			int localX = currentBlock.x - (chunkX * CHUNK_SIZE);
			int localY = currentBlock.y;
			int localZ = currentBlock.z - (chunkZ * CHUNK_SIZE);
			int index = chunk->GetBlockIndex(localX, localY, localZ);
			uint8_t block = chunk->Blocks[index];

			if (block != 0)
			{
				if (Mode)
				{
					if (hasEmptyBlock)
					{
						int placeGlobalX = lastEmptyBlock.x;
						int placeGlobalY = lastEmptyBlock.y;
						int placeGlobalZ = lastEmptyBlock.z;

						int placeChunkX = std::floor((float)placeGlobalX / CHUNK_SIZE);
						int placeChunkZ = std::floor((float)placeGlobalZ / CHUNK_SIZE);
						Chunk* placeChunk = World::world->Chunks[{placeChunkX, 0, placeChunkZ}];
						if (placeChunk)
						{
							int localPlaceX = placeGlobalX - (placeChunkX * CHUNK_SIZE);
							int localPlaceY = placeGlobalY;
							int localPlaceZ = placeGlobalZ - (placeChunkZ * CHUNK_SIZE);
							int placeIndex = placeChunk->GetBlockIndex(localPlaceX, localPlaceY, localPlaceZ);

							if (placeChunk->Blocks[placeIndex] == 0)
							{
								placeChunk->Blocks[placeIndex] = CurrentBlock; 
								placeChunk->UpdateChunk();
							}
						}
					}
				}
				else
				{
					chunk->Blocks[index] = BlockType::AIR;
					chunk->UpdateChunk();
				}
				break;
			}
			else
			{
				lastEmptyBlock = currentBlock;
				hasEmptyBlock = true;
			}
		}
	}
}
