#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "world.h"
#include "utils/shader.h"
#include "utils/camera.h"

void ProcessInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
inline void LoadTexture(const char* TexturePath);
inline float lerp(float a, float b, float t);

// Current Selected block for placement
static int SelectedBlock = BlockType::GRASS;

static const int windowWidth = 1280;
static const int windowHeight = 720;

static float dt = 0.0f;

static Camera camera(glm::ivec3(51, 32, -10), glm::vec2(windowWidth, windowHeight));

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Voxels", NULL, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, windowWidth, windowHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	Shader WorldShader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    LoadTexture("assets/gfx/textureatlas.png");

    World::world = new World();

    double LastTime = glfwGetTime();
    int FrameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        double CurrentTime = glfwGetTime();
        dt = CurrentTime - LastTime;
        LastTime = CurrentTime;

        ProcessInput(window);
        camera.Update(window, dt);
		World::world->Update(camera);

		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        WorldShader.SetMat4("view", camera.ViewMatrix());
        WorldShader.SetMat4("projection", camera.ProjectionMatrix());
        WorldShader.SetInt("TextureAtlas", 0);
		WorldShader.Use();

        World::world->Render(WorldShader);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    delete World::world;

    glfwTerminate();
    return 0;
}

void ProcessInput(GLFWwindow* window)
{
    // Place / Break
	RaycastHit RaycastPlace = World::world->Raycast(camera.Position, camera.Direction, 5, true);
	RaycastHit RaycastBreak = World::world->Raycast(camera.Position, camera.Direction, 5, false);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && RaycastPlace.WorldChunk != nullptr)
    {
		World::world->SetBlock(RaycastPlace.WorldChunk, RaycastPlace.BlockPosition, SelectedBlock, true);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && RaycastBreak.WorldChunk != nullptr)
    {
		World::world->SetBlock(RaycastBreak.WorldChunk, RaycastBreak.BlockPosition, SelectedBlock, false);
    }

	// "Sprint" / Dynamic FOV
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
	{
		camera.speed = 3000.0f * dt;
		camera.FOV = lerp(camera.FOV, 80.0f, 10.0f * dt);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		camera.speed = 1000.0f * dt;
		camera.FOV = lerp(camera.FOV, 60.0f, 10.0f * dt);
	}

    // Movement Stuffs
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.Position += camera.Direction * camera.speed * dt;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.Position -= camera.Direction * camera.speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.Position -= glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.Position += glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.Position += camera.Up * camera.speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.Position -= camera.Up * camera.speed * dt;
    }

    // Polygon Mode
    if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Close Window
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

inline void LoadTexture(const char* TexturePath)
{
    GLuint textureID;
    GLint width, height, nrChannels;

    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(TexturePath, &width, &height, &nrChannels, 0);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
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

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
