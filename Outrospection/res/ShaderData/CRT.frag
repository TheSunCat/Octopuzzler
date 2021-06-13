#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int time;

void main()
{
    FragColor = vec4(TexCoords, 1.0 , 1.0);
}  