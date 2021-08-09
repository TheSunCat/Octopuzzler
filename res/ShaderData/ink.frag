#version 330 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D image;
uniform int viewportHeight;

void main()
{
    color = texture(image, texCoords);
    color.rgb = mix(color.rgb, vec3(0.169,0.271,0.361), gl_FragCoord.y / float(viewportHeight));
}