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

RaycastHit World::Raycast(glm::vec3 Position, glm::vec3 Direction, float MaxReach, bool PlaceMode)
{
    float CurrentReach = 0.0f;
    glm::ivec3 LastEmptyBlock = glm::ivec3(std::floor(Position.x), std::floor(Position.y), std::floor(Position.z));

    while (CurrentReach < MaxReach)
    {
        CurrentReach += 0.01f;
        glm::vec3 Result = Position + Direction * CurrentReach;
        glm::ivec3 CurrentBlock(
            std::floor(Result.x),
            std::floor(Result.y),
            std::floor(Result.z)
        );

        if (!PlaceMode || (CurrentBlock != LastEmptyBlock))
        {
            int ChunkX = std::floor((float)CurrentBlock.x / CHUNK_SIZE);
            int ChunkZ = std::floor((float)CurrentBlock.z / CHUNK_SIZE);
            glm::ivec3 chunkPos(ChunkX, 0, ChunkZ);
            if (Chunks.find(chunkPos) != Chunks.end())
            {
                Chunk* chunk = World::world->Chunks[chunkPos];
                int LocalX = CurrentBlock.x - (ChunkX * CHUNK_SIZE);
                int LocalY = CurrentBlock.y;
                int LocalZ = CurrentBlock.z - (ChunkZ * CHUNK_SIZE);

                if (CurrentBlock.y > 0 && LastEmptyBlock.y < CHUNK_HEIGHT && chunk && chunk->Blocks[chunk->GetBlockIndex(LocalX, LocalY, LocalZ)] != BlockType::AIR)
                {
                    if (PlaceMode)
                    {
                        int LastChunkX = std::floor((float)LastEmptyBlock.x / CHUNK_SIZE);
                        int LastChunkZ = std::floor((float)LastEmptyBlock.z / CHUNK_SIZE);
                        glm::ivec3 lastChunkPos(LastChunkX, 0, LastChunkZ);
                        if (Chunks.find(lastChunkPos) != Chunks.end())
                        {
                            Chunk* lastChunk = World::world->Chunks[lastChunkPos];
                            int LastLocalX = LastEmptyBlock.x - (LastChunkX * CHUNK_SIZE);
                            int LastLocalY = LastEmptyBlock.y;
                            int LastLocalZ = LastEmptyBlock.z - (LastChunkZ * CHUNK_SIZE);
                            return RaycastHit(lastChunk, glm::ivec3(LastLocalX, LastLocalY, LastLocalZ));
                        }
                    }
                    else
                    {
                        return RaycastHit(chunk, glm::ivec3(LocalX, LocalY, LocalZ));
                    }
                }
            }
            if (PlaceMode)
            {
                LastEmptyBlock = CurrentBlock;
            }
        }
    }
    return RaycastHit(nullptr, glm::ivec3(0));
}

void World::SetBlock(Chunk* chunk, glm::ivec3 BlockPosition, int SelectedBlock, bool PlaceMode)
{
    int Index = chunk->GetBlockIndex(BlockPosition.x, BlockPosition.y, BlockPosition.z);
    
    if (PlaceMode)
    {
        chunk->Blocks[Index] = SelectedBlock; 
        chunk->UpdateChunk();
    }
    else if (!PlaceMode)
    {
        chunk->Blocks[Index] = BlockType::AIR;
        chunk->UpdateChunk();
    }
}