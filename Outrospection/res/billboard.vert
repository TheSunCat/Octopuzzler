#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 billboard_Center;

void main()
{
	TexCoords = vec2(aPos.x + 0.5, 1.0 - aPos.y);


	vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);

	vec3 vertexPosition_worldspace =
		billboard_Center
		+ CameraRight_worldspace * aPos.x * 1.0
		+ vec3(0, 1, 0) * aPos.y * 1.0;

	vec3 worldPos = vec3(model * vec4(vertexPosition_worldspace, 1.0));

    gl_Position = projection * view * vec4(worldPos, 1.0);

	
}