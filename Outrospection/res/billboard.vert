#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = vec2(aPos.x + 0.5, aPos.y);
	Normal = vec3(0.0);
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}