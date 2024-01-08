precision mediump float;

varying vec2 texCoords;

uniform sampler2D glyph;
uniform vec3 textColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(glyph, texCoords).a);
    gl_FragColor = sampled * vec4(textColor, 1.0);
}
