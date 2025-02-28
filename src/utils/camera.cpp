#include "camera.h"

Camera::Camera(glm::vec3 CameraPosition, glm::vec2 WindowSize)
{
    FOV = 60.0f;
    Yaw = -90.0f;
    Pitch = 0.0f;
    Sensitivity = 10.0f;

    ViewPlane = WindowSize;
    Position = CameraPosition;
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    PreviousMousePosition = glm::vec2(ViewPlane.x / 2, ViewPlane.y / 2);
}

void Camera::Update(GLFWwindow* window, f32 dt) 
{
    glfwGetCursorPos(window, &CurrentMousePosition.x, &CurrentMousePosition.y);

    glm::vec2 MousePositionDelta = CurrentMousePosition - PreviousMousePosition;
    PreviousMousePosition = CurrentMousePosition;

    Yaw += MousePositionDelta.x * Sensitivity * dt;
    Pitch -= MousePositionDelta.y * Sensitivity * dt;

    // Clamp Pitch to Normal Look Bounds
    if (Pitch > 89.0f) 
    {
        Pitch = 89.0f;
    }
    else if (Pitch < -89.0f) 
    {
        Pitch = -89.0f;
    }

    Direction = glm::normalize(glm::vec3(cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)), sin(glm::radians(Pitch)), sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))));
}

glm::mat4 Camera::ViewMatrix()
{
    return glm::lookAt(Position, Position + Direction, Up);
}

glm::mat4 Camera::ProjectionMatrix()
{
    return glm::perspective(glm::radians(FOV), (ViewPlane.x / ViewPlane.y), NEAR_PLANE, FAR_PLANE);
}