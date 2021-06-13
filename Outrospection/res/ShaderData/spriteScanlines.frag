#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform vec2 resolution;
uniform float time;

// shader from https://www.shadertoy.com/view/tssSzM, thanks!!

#define PI 3.14159265359

// ------ PARAMETERS ------
float crtOutIntensity = 1.1; // intensity of crt cell outline
float crtInIntensity = 0.9; // intensity of crt cell inside
float scanIntensity = 1.1; // intensity of scanlines
float aberrationIntensity = 1.5; // Intensity of chromatic aberration
int monochromeAberrations = 0;
// ------------------------


vec3 surface(vec2 uv) {
	return texture(screenTexture, uv).rgb;
}

// Scanlines chromatic aberration
vec3 aberration(vec2 uv) {
    float o = sin(uv.y * resolution.x * PI);
    o *= aberrationIntensity / resolution.x;
    vec3 newVec = vec3(surface(vec2( uv.x+o, uv.y+o )).x, surface(vec2( uv.x, uv.y+o )).y, surface(vec2( uv.x+o, uv.y )).z);
    if (monochromeAberrations > 0) {
        newVec = newVec / 3.0
            + vec3(surface(vec2( uv.x, uv.y+o )).x, surface(vec2( uv.x+o, uv.y+o )).y, surface(vec2( uv.x+o, uv.y )).z) / 3.0
            + vec3(surface(vec2( uv.x+o, uv.y )).x, surface(vec2( uv.x+o, uv.y )).y, surface(vec2( uv.x+o, uv.y+o )).z) / 3.0;
    }
    return newVec;
}

void main()
{
    vec2 fragCoord = texCoords * resolution;

    vec2 uv = texCoords;
    FragColor.rgb = aberration(uv);

    FragColor.w = 1.0; // not transparent
}