#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 viewProj;

uniform vec3 billboard_Center;
uniform vec3 cameraRight;

void main()
{
    TexCoords = vec2(aPos.x + 0.5, 1.0 - aPos.y);

    vec3 vertexPosition_worldspace =
        billboard_Center
        + cameraRight * aPos.x * 1.0
        + vec3(0, 1, 0) * aPos.y * 1.0;

    vec3 worldPos = vec3(model * vec4(vertexPosition_worldspace, 1.0));

    gl_Position = viewProj * vec4(worldPos, 1.0);
}