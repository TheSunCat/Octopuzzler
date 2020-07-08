#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 col;

void main()
{
    vec4 modelView = view * model * vec4(aPos, 1.0);
    
    col = vec3(-modelView.z / 96.0) + 0.1;

    gl_Position = projection * modelView;
}