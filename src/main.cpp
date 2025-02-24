#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "world.h"
#include "utils/shader.h"
#include "utils/camera.h"

void ProcessInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
inline void LoadTexture(const char* TexturePath);
inline void debugInfo(double currentTime, double& lastTime, int& frameCount, Camera camera);

// Current Selected block for placement
static int CurrentBlock = BlockType::GRASS;

static const int windowWidth = 1280;
static const int windowHeight = 720;

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

	Shader shader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    LoadTexture("assets/gfx/textureatlas.png");

    World::world = new World();

    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        debugInfo(glfwGetTime(), lastTime, frameCount, camera);

        ProcessInput(window);

        camera.Update(window);
		World::world->Update(camera);

        shader.SetMat4("view", camera.ViewMatrix());
        shader.SetMat4("projection", camera.ProjectionMatrix());
        shader.SetInt("TextureAtlas", 0);
		shader.Use();

		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        World::world->Render(shader);

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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
    {
        World::world->Raycast(camera.Position, camera.Direction, 5, CurrentBlock, true);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
    {
        World::world->Raycast(camera.Position, camera.Direction, 5, CurrentBlock, false);
    }

    // "Sprint"
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) 
    {
        camera.speed = 0.5f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) 
    {
        camera.speed = 0.1f;
    }

    // Movement Stuffs
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.Position += camera.Direction * camera.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.Position -= camera.Direction * camera.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.Position -= glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.Position += glm::normalize(glm::cross(camera.Direction, camera.Up)) * camera.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.Position += camera.Up * camera.speed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.Position -= camera.Up * camera.speed;
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

inline void debugInfo(double currentTime, double& lastTime, int& frameCount, Camera camera)
{
    frameCount++;
    if (currentTime - lastTime >= 1.0)
    {
        printf("FPS: %d\n", frameCount);
        printf("Camera Position: (%f, %f, %f)\n", camera.Position.x, camera.Position.y, camera.Position.z);
        
        switch (CurrentBlock)
        {
        case 1:
            std::cout << "Current Block: Grass" << std::endl;
            break;
        case 2:
            std::cout << "Current Block: Stone" << std::endl;
            break;
        case 3:
            std::cout << "Current Block: Snow" << std::endl;
            break;
        case 4:
            std::cout << "Current Block: Sand" << std::endl;
            break;
        case 5:
            std::cout << "Current Block: Wood" << std::endl;
            break;
        case 6:
            std::cout << "Current Block: Leaves" << std::endl;
            break;
        case 7:
            std::cout << "Current Block: Water" << std::endl;
            break;
        default:
            std::cout << "Current Block: NULL" << std::endl;
        }

        frameCount = 0;
        lastTime = currentTime;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset > 0)
    {
        CurrentBlock++;
        if (CurrentBlock > 7)
        {
            CurrentBlock = 1;
        }
    }
    else
    {
        CurrentBlock--;
        if (CurrentBlock < 1)
        {
            CurrentBlock = 7;
        }
    }
}