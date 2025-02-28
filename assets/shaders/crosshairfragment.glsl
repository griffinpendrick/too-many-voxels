#version 330 core
out vec4 FragColor;

in vec3 CrosshairColor;

void main()
{
    FragColor = vec4(CrosshairColor, 1.0);
}