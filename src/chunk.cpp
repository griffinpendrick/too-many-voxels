#include "chunk.h"
#include "chunkmanager.h"

inline u16 GetBlockIndex(const u8 x, const u8 y, const u8 z)
{
    return x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_HEIGHT);
}

void DeleteChunk(Chunk* chunk)
{
	glDeleteVertexArrays(1, &chunk->VAO);
	glDeleteBuffers(1, &chunk->VBO);
	glDeleteBuffers(1, &chunk->EBO);

	delete chunk;
}

inline u8 GetHeightMap(Chunk* chunk, const u8 x, const u8 z)
{
    FastNoiseLite noise(NOISE_SEED);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    f32 Height = 60.0f; // Base Height of Terrain
    f32 Amplitude = 30.0f;
    f32 Frequency = 0.003f;

	// Convert Local Block Position to World Position
    f32 BlockWorldX = (f32)(chunk->Position.x * CHUNK_SIZE + x);
    f32 BlockWorldZ = (f32)(chunk->Position.z * CHUNK_SIZE + z);

	// Apply Noise Octaves to Height Map
    for (u8 i = 0; i < NOISE_OCTAVES; i++)
    {
		noise.SetFrequency(Frequency);
		Height += noise.GetNoise(BlockWorldX, BlockWorldZ) * Amplitude;

        Amplitude *= 0.5f;
        Frequency *= 2.0f;
    }

    return (u8)Height;
}

void GenerateChunk(Chunk* chunk)
{
    for (u8 x = 0; x < CHUNK_SIZE; ++x)
    {
        for (u8 z = 0; z < CHUNK_SIZE; ++z)
        {
            u8 Height = GetHeightMap(chunk, x, z);
            for (u8 y = 0; y < Height; ++y)
            {
				u16 Index = GetBlockIndex(x, y, z);
				if (y == Height - 1)
				{
					if (y > 80)
					{
						chunk->Blocks[Index] = BlockType::SNOW;
					}
					else if (y < WATER_LEVEL)
					{
						chunk->Blocks[Index] = BlockType::WATER;
					}
					else if (y < 30)
					{
						chunk->Blocks[Index] = BlockType::SAND;
					}
					else
					{
						chunk->Blocks[Index] = BlockType::GRASS;
					}
				}
				else if (y == 0)
				{
					chunk->Blocks[Index] = BlockType::BEDROCK;
				}
				else
				{
					chunk->Blocks[Index] = BlockType::STONE;
				}
            }

            if (x == 10 && z == 10)
            {
                if (!(chunk->Blocks[GetBlockIndex(x, Height - 1, z)] == BlockType::SAND || chunk->Blocks[GetBlockIndex(x, Height - 1, z)] == BlockType::WATER))
                {
					for (u8 i = 0; i < 3; ++i)
					{
						for (u8 j = 0; j < 3; ++j)
						{
							// Generates Trees
							chunk->Blocks[GetBlockIndex(x, Height + i + j, z)] = BlockType::WOOD;
							chunk->Blocks[GetBlockIndex(x + i, Height + 4, z + j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x + i, Height + 4, z - j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x - i, Height + 4, z + j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x - i, Height + 4, z - j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x + i, Height + 5, z + j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x + i, Height + 5, z - j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x - i, Height + 5, z + j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x - i, Height + 5, z - j)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x + i - 1, Height + 6, z + j - 1)] = BlockType::LEAVES;
							chunk->Blocks[GetBlockIndex(x + i - 1, Height + 7, z + j - 1)] = BlockType::LEAVES;
						}
					}
                }
            }
        }
    }
}

void GenerateChunkMesh(Chunk* chunk)
{
    for (u8 x = 0; x < CHUNK_SIZE; ++x)
    {
		for (u8 y = 0; y < CHUNK_HEIGHT; ++y)
        {
			for (u8 z = 0; z < CHUNK_SIZE; ++z)
            {
                if (chunk->Blocks[GetBlockIndex(x, y, z)])
                {
                    GenerateBlockMesh(chunk, x, y, z);
                }
            }
        }
    }
}

void GenerateBlockMesh(Chunk* chunk, const u8 x, const u8 y, const u8 z)
{
    glm::vec3 p1(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p2(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p4(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p5(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p6(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p7(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p8(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

	// Gets Texture of the Block at (x, y, z)
	u8 TextureIndex = chunk->Blocks[GetBlockIndex(x, y, z)] - 1;

    // Front Face
    if (z == CHUNK_SIZE - 1 || !chunk->Blocks[GetBlockIndex(x, y, z + 1)])
    {
		AddFace(chunk, p1, p2, p3, p4, glm::vec3(0.0f, 0.0f, 1.0f), UVTable[TextureIndex].Side);
	}

    // Back Face
    if (z == 0 || !chunk->Blocks[GetBlockIndex(x, y, z - 1)])
    {
		AddFace(chunk, p5, p6, p7, p8, glm::vec3(0.0f, 0.0f, -1.0f), UVTable[TextureIndex].Side);
	}

    // Right Face
    if (x == CHUNK_SIZE - 1 || !chunk->Blocks[GetBlockIndex(x + 1, y, z)])
    {
		AddFace(chunk, p2, p5, p8, p3, glm::vec3(1.0f, 0.0f, 0.0f), UVTable[TextureIndex].Side);
	}

    // Left Face
    if (x == 0 || !chunk->Blocks[GetBlockIndex(x - 1, y, z)])
    {
		AddFace(chunk, p6, p1, p4, p7, glm::vec3(-1.0f, 0.0f, 0.0f), UVTable[TextureIndex].Side);
	}

    // Top Face 
    if (y == CHUNK_HEIGHT - 1 || !chunk->Blocks[GetBlockIndex(x, y + 1, z)])
    {
        AddFace(chunk, p4, p3, p8, p7, glm::vec3(0.0f, 1.0f, 0.0f), UVTable[TextureIndex].Top);
    }

    // Bottom Face
    if (y == 0 || !chunk->Blocks[GetBlockIndex(x, y - 1, z)])
    {
		AddFace(chunk, p6, p5, p2, p1, glm::vec3(0.0f, -1.0f, 0.0f), UVTable[TextureIndex].Bottom);
    }
}

void UpdateChunk(Chunk* chunk) 
{
	// Clear Buffers in Case of Remesh
	chunk->Vertices.clear();
	chunk->Indices.clear();

    GenerateChunkMesh(chunk);

	glGenVertexArrays(1, &chunk->VAO);
	glBindVertexArray(chunk->VAO);

	glGenBuffers(1, &chunk->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->VBO);
	glBufferData(GL_ARRAY_BUFFER, chunk->Vertices.size() * sizeof(Vertex), chunk->Vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &chunk->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk->Indices.size() * sizeof(u32), chunk->Indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

inline void AddFace(Chunk* chunk, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4, glm::vec3& Normal, const glm::vec2 UV[])
{
    size_t Index = chunk->Vertices.size();

	chunk->Vertices.push_back({UV[0], p1, Normal});
	chunk->Vertices.push_back({UV[1], p2, Normal});
	chunk->Vertices.push_back({UV[2], p3, Normal});
	chunk->Vertices.push_back({UV[3], p4, Normal});

	chunk->Indices.push_back((u32)Index + 0);
	chunk->Indices.push_back((u32)Index + 1);
	chunk->Indices.push_back((u32)Index + 2);
	chunk->Indices.push_back((u32)Index + 0);
	chunk->Indices.push_back((u32)Index + 2);
	chunk->Indices.push_back((u32)Index + 3);
}