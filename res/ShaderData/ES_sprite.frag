precision mediump float;

varying vec2 texCoords;

uniform sampler2D image;

void main()
{    
    gl_FragColor = texture2D(image, texCoords);
}
