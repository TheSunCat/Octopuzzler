#version 330 core
layout (location = 0) in vec2 pos;

out vec2 texCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    texCoords = pos;
    gl_Position = projection * model * vec4(pos, 0.0, 1.0);
}