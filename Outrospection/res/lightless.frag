#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D diffuseTex;

void main()
{
    FragColor = texture(diffuseTex, TexCoords);
} 