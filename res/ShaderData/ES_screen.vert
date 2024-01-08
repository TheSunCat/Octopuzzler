precision mediump float;

attribute /*layout (location = 0) in*/ vec2 aPos;
attribute /*layout (location = 1) in*/ vec2 aTexCoords;

varying vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  
