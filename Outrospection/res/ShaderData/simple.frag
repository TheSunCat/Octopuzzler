#version 330 core
out vec4 FragColor;

in vec3 col;

void main()
{
    FragColor = vec4(col, 1.0);
} 