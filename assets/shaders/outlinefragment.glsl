#version 330 core
out vec4 FragColor;

in vec3 OutlineColor;

void main()
{
    FragColor = vec4(OutlineColor, 1.0);
}