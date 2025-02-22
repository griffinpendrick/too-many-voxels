#include "camera.h"

Camera::Camera(glm::vec3 pos, glm::vec2 WindowSize)
{
    sensitivity = 0.1f;
    yaw = -90.0f;
    pitch = 0.0f;

    ViewPlane = WindowSize;
    PreviousMousePosition = glm::vec2(ViewPlane.x / 2, ViewPlane.y / 2);

    Position = pos;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::Update(GLFWwindow* window) 
{
    glfwGetCursorPos(window, &CurrentMousePosition.x, &CurrentMousePosition.y);

    glm::vec2 MousePositionDelta = CurrentMousePosition - PreviousMousePosition;
    PreviousMousePosition = CurrentMousePosition;

    yaw += MousePositionDelta.x * sensitivity;
    pitch -= MousePositionDelta.y * sensitivity;

    if (pitch > 89.0f) 
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) 
    {
        pitch = -89.0f;
    }

    Direction = glm::normalize(glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), sin(glm::radians(yaw)) * cos(glm::radians(pitch))));
}

glm::mat4 Camera::ViewMatrix()
{
    return glm::lookAt(Position, Position + Direction, Up);
}

glm::mat4 Camera::ProjectionMatrix()
{
    return glm::perspective(glm::radians(FOV), (ViewPlane.x / ViewPlane.y), 0.1f, 10000.0f);
}