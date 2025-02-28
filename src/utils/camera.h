#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "common.h"

#define NEAR_PLANE 0.1f
#define FAR_PLANE 10000.0f

struct Camera
{
    Camera(glm::vec3 CameraPosition, glm::vec2 WindowSize);

    void Update(GLFWwindow* window, f32 dt);
    glm::mat4 ViewMatrix();
    glm::mat4 ProjectionMatrix();

    f32 Speed; // Camera Movement Speed
    f32 Sensitivity; // Camera Look Sensitivity
    f32 FOV;
    f32 Pitch;
    f32 Yaw;

    glm::vec2 ViewPlane;

    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Up;

    glm::dvec2 CurrentMousePosition;
    glm::dvec2 PreviousMousePosition;
};