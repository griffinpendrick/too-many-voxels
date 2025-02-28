#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "world.h"
#include "utils/common.h"
#include "utils/shader.h"
#include "utils/camera.h"

static f32 WindowWidth = 1280.0f;
static f32 WindowHeight = 720.0f;
#define ASPECT_RATIO WindowWidth / WindowHeight

static f32 dt = 0.0f;
static u8 SelectedBlock = BlockType::GRASS; // The Current Block the Player is "Holding"

static Camera camera(glm::ivec3(51, 32, -10), glm::vec2(WindowWidth, WindowHeight));

static RaycastInfo RaycastHit(nullptr, nullptr, glm::ivec3(0), glm::ivec3(0));
#define PLAYER_REACH 5.0f

void ProcessInput(GLFWwindow* Window)
{
    // Place / Break Voxel
    if (RaycastHit.CurrentChunk)
    {
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{
			World::world->SetBlock(RaycastHit.RayChunk, RaycastHit.PlacePosition, SelectedBlock, true);
		}
		if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			World::world->SetBlock(RaycastHit.CurrentChunk, RaycastHit.BreakPosition, SelectedBlock, false);
		}
    }

	// "Sprint" & Dynamic FOV Change
	if (glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
	{
		camera.Speed = 3000.0f * dt;
		camera.FOV = lerp(camera.FOV, 80.0f, 10.0f * dt);
	}
	else
    {
		camera.Speed = 1000.0f * dt;
		camera.FOV = lerp(camera.FOV, 60.0f, 10.0f * dt);
	}

    // WASD Movement
    if (glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.Position += camera.Direction * camera.Speed * dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.Position -= glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.Speed * dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.Position -= camera.Direction * camera.Speed * dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.Position += glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.Speed * dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.Position += camera.Up * camera.Speed * dt;
    }
    if (glfwGetKey(Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.Position -= camera.Up * camera.Speed * dt;
    }

    // Close Window
    if(glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(Window, GLFW_TRUE);
    }
}

inline void LoadTexture(const char* TexturePath)
{
    u32 ID;
    s32 Width, Height, Channels;

    stbi_set_flip_vertically_on_load(1); // Flips the Textures to the Correct Orientation
    unsigned char* data = stbi_load(TexturePath, &Width, &Height, &Channels, 0);

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void framebuffer_size_callback(GLFWwindow* Window, s32 Width, s32 Height)
{
    // Update Globals
    WindowWidth = Width;
    WindowHeight = Height;

    glViewport(0, 0, Width, Height);
}  

void scroll_callback(GLFWwindow* Window, f64 XOffset, f64 YOffset)
{
    // Clamp Players Block Selection
    if (YOffset > 0)
    {
        SelectedBlock++;
        if (SelectedBlock > 8)
        {
            SelectedBlock = 1;
        }
    }
    else
    {
        SelectedBlock--;
        if (SelectedBlock < 1)
        {
            SelectedBlock = 8;
        }
    }
}

void RenderPlacementOutline(Chunk* chunk, glm::ivec3 position)
{
	position += (chunk->Position * CHUNK_SIZE); // Converts Block Position From Local to World Coords

    glm::vec3 OutlineVertices[] = {
        glm::vec3(position.x - BLOCK_RENDER_SIZE, position.y - BLOCK_RENDER_SIZE, position.z + BLOCK_RENDER_SIZE), 
        glm::vec3(position.x + BLOCK_RENDER_SIZE, position.y - BLOCK_RENDER_SIZE, position.z + BLOCK_RENDER_SIZE), 
        glm::vec3(position.x + BLOCK_RENDER_SIZE, position.y + BLOCK_RENDER_SIZE, position.z + BLOCK_RENDER_SIZE), 
        glm::vec3(position.x - BLOCK_RENDER_SIZE, position.y + BLOCK_RENDER_SIZE, position.z + BLOCK_RENDER_SIZE), 
        glm::vec3(position.x - BLOCK_RENDER_SIZE, position.y - BLOCK_RENDER_SIZE, position.z - BLOCK_RENDER_SIZE), 
        glm::vec3(position.x + BLOCK_RENDER_SIZE, position.y - BLOCK_RENDER_SIZE, position.z - BLOCK_RENDER_SIZE), 
        glm::vec3(position.x + BLOCK_RENDER_SIZE, position.y + BLOCK_RENDER_SIZE, position.z - BLOCK_RENDER_SIZE), 
        glm::vec3(position.x - BLOCK_RENDER_SIZE, position.y + BLOCK_RENDER_SIZE, position.z - BLOCK_RENDER_SIZE)  
    };

    u32 indices[] = {
        0, 1, 1, 2, 
        2, 3, 3, 0, 
        4, 5, 5, 6, 
        6, 7, 7, 4, 
        0, 4, 1, 5, 
        2, 6, 3, 7  
    };

    u32 VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), OutlineVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(u32), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glLineWidth(2.5);    
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void RenderCrosshair()
{
    f32 CenterX = WindowWidth / 2.0f;
    f32 CenterY = WindowHeight / 2.0f;

	f32 CrosshairVertices[] =
	{
		CenterX - 7.0f, CenterY,
		CenterX + 7.0f, CenterY,
		CenterX,        CenterY - 7.0f,
		CenterX,        CenterY + 7.0f,
	};

    u32 VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CrosshairVertices), CrosshairVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    glLineWidth(2.5); 
    glDrawArrays(GL_LINES, 0, 4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
