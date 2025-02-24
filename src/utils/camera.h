#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera
{
    Camera(glm::vec3 pos, glm::vec2 WindowSize);

    void Update(GLFWwindow* window, float dt);
    glm::mat4 ViewMatrix();
    glm::mat4 ProjectionMatrix();

    float speed;
    float sensitivity;
    float pitch;
    float yaw;
    float FOV;

    glm::vec2 ViewPlane;

    glm::dvec2 CurrentMousePosition;
    glm::dvec2 PreviousMousePosition;

    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Up;
};