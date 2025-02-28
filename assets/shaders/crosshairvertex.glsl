#version 330 core
layout (location = 0) in vec2 aPos;

out vec3 CrosshairColor;

uniform vec3 Color;
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 0.0, 1.0);
    CrosshairColor = Color;
}