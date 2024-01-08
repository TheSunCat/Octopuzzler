precision mediump float;

attribute /*layout (location = 0) in*/ vec2 pos;

varying vec2 texCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    texCoords = pos;
    gl_Position = projection * model * vec4(pos, 0.0, 1.0);
}
