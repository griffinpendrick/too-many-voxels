#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D TextureAtlas;

vec3 Ambient = vec3(0.5, 0.5, 0.5);

void main() 
{
	vec3 LightDir = normalize(vec3(0.5, 1, 0.5));
	vec3 Diffuse = max(dot(Normal, LightDir), 0.0) * vec3(1);

	vec3 Result = Ambient + Diffuse;
    vec4 TextureColor = texture(TextureAtlas, TexCoord);

    FragColor = TextureColor * vec4(Result, 1.0);
}
