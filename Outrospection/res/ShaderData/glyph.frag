#version 330 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D glyph;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(glyph, texCoords).r);
    color = sampled * vec4(textColor, 1.0);
}