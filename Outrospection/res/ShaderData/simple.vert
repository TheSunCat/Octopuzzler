#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 viewProj;

out vec3 col;

void main()
{
    vec4 modelViewProj = viewProj * model * vec4(aPos, 1.0);
    
    col = vec3(1);//vec3(-modelViewProj.z / 96.0) + 0.1; // TODO not actually depth (need modelView, w/o proj)

    gl_Position = modelViewProj;
}