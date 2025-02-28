#include "main.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "Too Many Voxels", NULL, NULL);
    if (!Window) return -1;

    glfwMakeContextCurrent(Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(Window, WindowWidth / 2.0, WindowHeight / 2.0);
    glViewport(0, 0, WindowWidth, WindowHeight);

	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);  
	glfwSetScrollCallback(Window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

	Shader WorldShader("assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");
    Shader OutlineShader("assets/shaders/outlinevertex.glsl", "assets/shaders/outlinefragment.glsl");
    Shader CrosshairShader("assets/shaders/crosshairvertex.glsl", "assets/shaders/crosshairfragment.glsl");
    LoadTexture("assets/gfx/textureatlas.png");

    World::world = new World();

    f64 LastTime = glfwGetTime();
    f64 CurrentTime = 0.0;

    while (!glfwWindowShouldClose(Window))
    {
        CurrentTime = glfwGetTime();
        dt = (f32)(CurrentTime - LastTime);
        LastTime = CurrentTime;

        ProcessInput(Window); // Process Players Input
        camera.Update(Window, dt); // Process Camera via Players Mouse Movement
		World::world->Update(camera); // World Loading Updates

		RaycastHit = World::world->Raycast(camera.Position, camera.Direction, PLAYER_REACH); // Find Block Player is Looking at

		glClearColor(0.2f, 0.5f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render Crosshair
		CrosshairShader.Use();
		CrosshairShader.SetVec3("Color", glm::vec3(0.0f));
        CrosshairShader.SetMat4("View", glm::mat4(1.0f));
		CrosshairShader.SetMat4("Model", glm::mat4(1.0f));
		CrosshairShader.SetMat4("Projection", glm::ortho(0.0f, WindowWidth, 0.0f, WindowHeight));
		RenderCrosshair();

        // Render Block Selection Outline
        if (RaycastHit.CurrentChunk && RaycastHit.RayChunk)
        {
            OutlineShader.Use();
			OutlineShader.SetVec3("Color", glm::vec3(0.0f));
            OutlineShader.SetMat4("View", camera.ViewMatrix());
            OutlineShader.SetMat4("Model", glm::mat4(1.0f));
            OutlineShader.SetMat4("Projection", camera.ProjectionMatrix());
            RenderPlacementOutline(RaycastHit.CurrentChunk, RaycastHit.BreakPosition);
        }

        // Render World
		WorldShader.Use();
        WorldShader.SetMat4("View", camera.ViewMatrix());
        WorldShader.SetMat4("Projection", camera.ProjectionMatrix());
        WorldShader.SetInt("TextureAtlas", 0);
        World::world->Render(WorldShader);

        glfwSwapBuffers(Window);
        glfwPollEvents();    
    }

    delete World::world;

    glfwTerminate();
    return 0;
}
