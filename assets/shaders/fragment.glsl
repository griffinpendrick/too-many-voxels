#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D TextureAtlas;

vec3 ambient = vec3(0.5, 0.5, 0.5);
vec3 lightDirection = vec3(0.5, 1, 0.5);

void main() 
{
	vec3 lightDir = normalize(lightDirection);
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1);

	vec3 result = ambient + diffuse;
    vec4 textureColor = texture(TextureAtlas, TexCoord);

    FragColor = textureColor * vec4(result, 1.0);
}
