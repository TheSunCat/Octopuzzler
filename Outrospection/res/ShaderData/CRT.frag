#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int time;

void main()
{
    FragColor = vec4(vec3(0.5), 1.0);
}  