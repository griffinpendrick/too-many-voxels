#include "chunk.h"
#include "world.h"

inline u16 GetBlockIndex(u8 x, u8 y, u8 z)
{
    return x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_HEIGHT);
}

Chunk::Chunk(glm::ivec3 ChunkPosition)
{
    Position = ChunkPosition;
    Generated = false;

	VAO = 0;
	VBO = 0;
	EBO = 0;

    memset(Blocks, BlockType::AIR, (CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE) * sizeof(u8));
}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

inline u8 Chunk::GetHeightMap(Chunk* chunk, u8 x, u8 z)
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

void Chunk::GenerateChunk()
{
    for (u8 x = 0; x < CHUNK_SIZE; ++x)
    {
        for (u8 z = 0; z < CHUNK_SIZE; ++z)
        {
            u8 Height = GetHeightMap(this, x, z);
            for (u8 y = 0; y < Height; ++y)
            {
				u16 Index = GetBlockIndex(x, y, z);
				if (y == Height - 1)
				{
					if (y > 80)
					{
						Blocks[Index] = BlockType::SNOW;
					}
					else if (y < WATER_LEVEL)
					{
						Blocks[Index] = BlockType::WATER;
					}
					else if (y < 30)
					{
						Blocks[Index] = BlockType::SAND;
					}
					else
					{
						Blocks[Index] = BlockType::GRASS;
					}
				}
				else if (y == 0)
				{
					Blocks[Index] = BlockType::BEDROCK;
				}
				else
				{
					Blocks[Index] = BlockType::STONE;
				}
            }

            if (x == 10 && z == 10)
            {
                if (!(Blocks[GetBlockIndex(x, Height - 1, z)] == BlockType::SAND || Blocks[GetBlockIndex(x, Height - 1, z)] == BlockType::WATER))
                {
                    GenerateTreeMesh(x, Height, z);
                }
            }
        }
    }
    Generated = true;
}

void Chunk::GenerateChunkMesh()
{
	// Clear Buffers in Case of Remesh
	vertices.clear();
	indices.clear();

    for (u8 x = 0; x < CHUNK_SIZE; ++x)
    {
		for (u8 y = 0; y < CHUNK_HEIGHT; ++y)
        {
			for (u8 z = 0; z < CHUNK_SIZE; ++z)
            {
                if (Blocks[GetBlockIndex(x, y, z)])
                {
                    GenerateBlockMesh(x, y, z);
                }
            }
        }
    }
}

inline void Chunk::GenerateTreeMesh(u8 x, u8 y, u8 z)
{
	for (u8 i = 0; i < 3; ++i)
	{
		for (u8 j = 0; j < 3; ++j)
		{
			Blocks[GetBlockIndex(x, y + i + j, z)] = BlockType::WOOD;

			Blocks[GetBlockIndex(x + i, y + 4, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 4, z - j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 4, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 4, z - j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 5, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 5, z - j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 5, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 5, z - j)] = BlockType::LEAVES;

			Blocks[GetBlockIndex(x + i - 1, y + 6, z + j - 1)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i - 1, y + 7, z + j - 1)] = BlockType::LEAVES;
		}
	}
}


void Chunk::GenerateBlockMesh(u8 x, u8 y, u8 z)
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
	u8 TextureIndex = Blocks[GetBlockIndex(x, y, z)] - 1;

    // Front Face
    if (z == CHUNK_SIZE - 1 || !Blocks[GetBlockIndex(x, y, z + 1)])
    {
		AddFace(p1, p2, p3, p4, glm::vec3(0.0f, 0.0f, 1.0f), UVTable[TextureIndex].Side);
	}

    // Back Face
    if (z == 0 || !Blocks[GetBlockIndex(x, y, z - 1)])
    {
		AddFace(p5, p6, p7, p8, glm::vec3(0.0f, 0.0f, -1.0f), UVTable[TextureIndex].Side);
	}

    // Right Face
    if (x == CHUNK_SIZE - 1 || !Blocks[GetBlockIndex(x + 1, y, z)])
    {
		AddFace(p2, p5, p8, p3, glm::vec3(1.0f, 0.0f, 0.0f), UVTable[TextureIndex].Side);
	}

    // Left Face
    if (x == 0 || !Blocks[GetBlockIndex(x - 1, y, z)])
    {
		AddFace(p6, p1, p4, p7, glm::vec3(-1.0f, 0.0f, 0.0f), UVTable[TextureIndex].Side);
	}

    // Top Face 
    if (y == CHUNK_HEIGHT - 1 || !Blocks[GetBlockIndex(x, y + 1, z)])
    {
        AddFace(p4, p3, p8, p7, glm::vec3(0.0f, 1.0f, 0.0f), UVTable[TextureIndex].Top);
    }

    // Bottom Face
    if (y == 0 || !Blocks[GetBlockIndex(x, y - 1, z)])
    {
		AddFace(p6, p5, p2, p1, glm::vec3(0.0f, -1.0f, 0.0f), UVTable[TextureIndex].Bottom);
    }
}

void Chunk::UpdateChunk() 
{
    GenerateChunkMesh();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

inline void Chunk::AddFace(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4, glm::vec3& Normal, const glm::vec2 UV[])
{
    size_t Index = vertices.size();

    vertices.push_back({UV[0], p1, Normal});
	vertices.push_back({UV[1], p2, Normal});
	vertices.push_back({UV[2], p3, Normal});
	vertices.push_back({UV[3], p4, Normal});
	
	indices.push_back((u32)Index + 0);
	indices.push_back((u32)Index + 1);
	indices.push_back((u32)Index + 2);
	indices.push_back((u32)Index + 0);
	indices.push_back((u32)Index + 2);
	indices.push_back((u32)Index + 3);
}