#include "world.h"

World* World::world = nullptr;

World::World()
{
    World::world = this;
}

void World::Update(Camera camera)
{
    int ChunkX = (int)(camera.Position.x / CHUNK_SIZE);
    int ChunkZ = (int)(camera.Position.z / CHUNK_SIZE);

    LoadChunks(ChunkX, ChunkZ);
    UnloadChunks(ChunkX, ChunkZ);

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
    for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; ++x)
    {
        for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; ++z)
        {
            glm::vec3 pos(PlayerChunkX + x, 0, PlayerChunkZ + z);
            if (Chunks.find(pos) == Chunks.end())
            {
                Chunks[pos] = new Chunk(pos);
                Chunks[pos]->GenerateChunk();
                ChunkQueue.push(pos);
            }
        }
    }
}

inline void World::UnloadChunks(int PlayerChunkX, int PlayerChunkZ) {
    for (auto it = Chunks.begin(); it != Chunks.end(); )
    {
        int chunkX = it->first.x;
        int chunkZ = it->first.z;

        if (std::abs(chunkX - PlayerChunkX) > RENDER_DISTANCE || std::abs(chunkZ - PlayerChunkZ) > RENDER_DISTANCE)
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
        glm::vec3 WorldPosition(chunk->Position.x * CHUNK_SIZE, chunk->Position.y * CHUNK_HEIGHT, chunk->Position.z * CHUNK_SIZE);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), WorldPosition);

        shader.SetMat4("model", model);

        glBindVertexArray(chunk->VAO);
        glDrawElements(GL_TRIANGLES, chunk->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}