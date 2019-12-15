#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D tex;

void main()
{
	vec4 color = texture(tex, TexCoords);

	if(color.w < 0.9)
		discard;

    FragColor = color;
}