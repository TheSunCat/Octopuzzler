#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 texCoords;

void main()
{
    texCoords = vec2((aPos.x + 0.5), (aPos.y + 0.66) / 1.32);
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  