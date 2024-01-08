precision mediump float;

varying vec2 texCoords;

uniform sampler2D image;
uniform int viewportHeight;

void main()
{
    vec4 color = texture2D(image, texCoords);
    color.rgb = mix(color.rgb, vec3(0.169,0.271,0.361), gl_FragCoord.y / float(viewportHeight));

    gl_FragColor = color;
}
