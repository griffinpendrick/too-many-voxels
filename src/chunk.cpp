#include "chunk.h"
#include "world.h"

Chunk::Chunk(glm::ivec3 pos)
{
    Position = pos;
    Generated = false;

    Blocks.resize(CHUNK_SIZE * CHUNK_HEIGHT * CHUNK_SIZE);
    memset(Blocks.data(), BlockType::AIR, Blocks.size() * sizeof(uint8_t));
}

inline int Chunk::GetBlockIndex(int x, int y, int z)
{
    return x + (y * CHUNK_SIZE) + (z * CHUNK_SIZE * CHUNK_HEIGHT);
}

inline float Chunk::GetHeightMap(Chunk* chunk, int x, int z)
{
    int WorldX = chunk->Position.x * CHUNK_SIZE + x;
    int WorldZ = chunk->Position.z * CHUNK_SIZE + z;

    FastNoiseLite noise;
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetSeed(999);

    float Height = 60.0f;
    float Amplitude = 30.0f;
    float Frequency = 0.003f;

    for (int i = 0; i < 4; i++)
    {
		noise.SetFrequency(Frequency);
		Height += noise.GetNoise((float)WorldX, (float)WorldZ) * Amplitude;
        Amplitude *= 0.5f;
        Frequency *= 2.0f;
    }

    return Height;
}

void Chunk::GenerateChunk()
{
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE; ++z)
        {
            int Height = GetHeightMap(this, x, z);
            for (int y = 0; y < Height; ++y)
            {
				if (y == Height - 1)
				{
					if (y > 80)
					{
						Blocks[GetBlockIndex(x, y, z)] = BlockType::SNOW;
					}
					else if (Height < WATER_LEVEL)
					{
						Blocks[GetBlockIndex(x, y, z)] = BlockType::WATER;
					}
					else if (Height < 30)
					{
						Blocks[GetBlockIndex(x, y, z)] = BlockType::SAND;
					}
					else
					{
						Blocks[GetBlockIndex(x, y, z)] = BlockType::GRASS;
					}
				}
				else
				{
					Blocks[GetBlockIndex(x, y, z)] = BlockType::STONE;
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
    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
		for (int y = 0; y < CHUNK_HEIGHT; ++y)
        {
			for (int z = 0; z < CHUNK_SIZE; ++z)
            {
                if (Blocks[GetBlockIndex(x, y, z)])
                {
                    GenerateBlockMesh(x, y, z);
                }
            }
        }
    }
}

void Chunk::GenerateBlockMesh(int x, int y, int z)
{
    int index = GetBlockIndex(x, y, z);
    
    glm::vec3 p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
    glm::vec3 p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
    glm::vec3 p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

    glm::vec2 uv1, uv2, uv3, uv4;

    // Side Textures
    if (Blocks[index] == BlockType::GRASS)
    {
		uv1 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
		uv2 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 2.0f};
		uv3 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 3.0f};
		uv4 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 3.0f};
    }
    else if (Blocks[index] == BlockType::STONE)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 3.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 3.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 2.0f};
    }
    else if (Blocks[index] == BlockType::SNOW)
    {
        uv1 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 2.0f};
        uv4 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
    }
    else if (Blocks[index] == BlockType::SAND)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WOOD)
    {
        uv4 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::LEAVES)
    {
        uv4 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WATER)
    {
        uv4 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
    }

    // Front Face
    if (z == CHUNK_SIZE - 1 || !Blocks[GetBlockIndex(x, y, z + 1)])
    {
		AddFace(p1, p2, p3, p4, glm::vec3(0.0f, 0.0f, 1.0f), uv1, uv2, uv3, uv4);
	}

    // Back Face
    if (z == 0 || !Blocks[GetBlockIndex(x, y, z - 1)])
    {
		AddFace(p5, p6, p7, p8, glm::vec3(0.0f, 0.0f, -1.0f), uv1, uv2, uv3, uv4);
	}

    // Right Face
    if (x == CHUNK_SIZE - 1 || !Blocks[GetBlockIndex(x + 1, y, z)])
    {
		AddFace(p2, p5, p8, p3, glm::vec3(1.0f, 0.0f, 0.0f), uv1, uv2, uv3, uv4);
	}

    // Left Face
    if (x == 0 || !Blocks[GetBlockIndex(x - 1, y, z)])
    {
		AddFace(p6, p1, p4, p7, glm::vec3(-1.0f, 0.0f, 0.0f), uv1, uv2, uv3, uv4);
	}

	// Top Texture
    if (Blocks[index] == BlockType::GRASS)
    {
		uv1 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 3.0f};
		uv2 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 3.0f};
		uv3 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 2.0f};
		uv4 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 2.0f};
    }
    else if (Blocks[index] == BlockType::STONE)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 3.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 3.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 2.0f};
    }
    else if (Blocks[index] == BlockType::SNOW)
    {
       uv1 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 2.0f};
       uv2 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 2.0f};
       uv3 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
       uv4 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
    }
    else if (Blocks[index] == BlockType::SAND)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WOOD)
    {
        uv4 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::LEAVES)
    {
        uv4 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WATER)
    {
        uv4 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
    }

    // Top Face 
    if (y == CHUNK_HEIGHT - 1 || !Blocks[GetBlockIndex(x, y + 1, z)])
    {
        AddFace(p4, p3, p8, p7, glm::vec3(0.0f, 1.0f, 0.0f), uv1, uv2, uv3, uv4);
    }

	// Bottom Texture
    if (Blocks[index] == BlockType::GRASS)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 2.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 1.0f};
    }
    else if (Blocks[index] == BlockType::STONE)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 3.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 3.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 2.0f};
    }
    else if (Blocks[index] == BlockType::SNOW)
    {
		uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 2.0f};
		uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 2.0f};
		uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
		uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 1.0f};
    }
    else if (Blocks[index] == BlockType::SAND)
    {
        uv1 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
        uv4 = {TEXTURE_DIMENSION * 0.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WOOD)
    {
        uv4 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 1.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::LEAVES)
    {
        uv4 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 2.0f, TEXTURE_DIMENSION * 0.0f};
    }
    else if (Blocks[index] == BlockType::WATER)
    {
        uv4 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 1.0f};
        uv3 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 1.0f};
        uv2 = {TEXTURE_DIMENSION * 4.0f, TEXTURE_DIMENSION * 0.0f};
        uv1 = {TEXTURE_DIMENSION * 3.0f, TEXTURE_DIMENSION * 0.0f};
    }

    // Bottom Face
    if (y == 0 || !Blocks[GetBlockIndex(x, y - 1, z)])
    {
		AddFace(p6, p5, p2, p1, glm::vec3(0.0f, -1.0f, 0.0f), uv1, uv2, uv3, uv4);
    }
}

inline void Chunk::GenerateTreeMesh(int x, int y, int z)
{
	// Generate Tree Trunk
	Blocks[GetBlockIndex(x, y, z)] = BlockType::WOOD;
	Blocks[GetBlockIndex(x, y + 1, z)] = BlockType::WOOD;
	Blocks[GetBlockIndex(x, y + 2, z)] = BlockType::WOOD;
	Blocks[GetBlockIndex(x, y + 3, z)] = BlockType::WOOD;
	Blocks[GetBlockIndex(x, y + 4, z)] = BlockType::WOOD;
	Blocks[GetBlockIndex(x, y + 5, z)] = BlockType::WOOD;

	// Generate Leaves
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Blocks[GetBlockIndex(x + i, y + 4, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 4, z - j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 4, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 4, z - j)] = BlockType::LEAVES;

			Blocks[GetBlockIndex(x + i, y + 5, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 5, z - j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 5, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x - i, y + 5, z - j)] = BlockType::LEAVES;
		}
	}
	
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			Blocks[GetBlockIndex(x + i, y + 6, z + j)] = BlockType::LEAVES;
			Blocks[GetBlockIndex(x + i, y + 7, z + j)] = BlockType::LEAVES;
		}
	}
}

void Chunk::UpdateChunk() 
{
    GenerateChunkMesh();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

inline void Chunk::AddFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4,
						   const glm::vec3& normal,
						   const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4)
{
    int index = vertices.size();

	vertices.push_back({p1, normal, uv1});
	vertices.push_back({p2, normal, uv2});
	vertices.push_back({p3, normal, uv3});
	vertices.push_back({p4, normal, uv4});
	
	indices.push_back(index + 0);
	indices.push_back(index + 1);
	indices.push_back(index + 2);
	indices.push_back(index + 0);
	indices.push_back(index + 2);
	indices.push_back(index + 3);
}
