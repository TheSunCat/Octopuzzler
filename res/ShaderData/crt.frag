#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
//uniform vec2 resolution;
uniform float time;

// math from https://www.shadertoy.com/view/Ms23DR and https://www.shadertoy.com/view/wddyRH, thanks!!
#define PI 3.14159265359

// from rez in Glenz vector form Hell
float rand(in vec2 p,in float t) {
	return fract(sin(dot(p+mod(t,1.0),vec2(12.9898,78.2333)))*43758.5453);
}


vec2 curve(vec2 uv)
{
	uv = (uv - 0.5) * 2.0;
	uv *= 1.1;	
	uv.x *= 1.0 + pow((abs(uv.y) / 5.0), 2.0);
	uv.y *= 1.0 + pow((abs(uv.x) / 4.0), 2.0);
	uv  = (uv / 2.0) + 0.5;
	uv =  uv *0.92 + 0.04;
	return uv;
}

void main()
{
    vec3 col = texture(screenTexture, curve(texCoords)).xyz;

    // line
    float vig = 16.*texCoords.x * texCoords.y * (1.-texCoords.x)*(1.-texCoords.y);
	col *= pow(vig, .2);

    FragColor = vec4(col, 1.0);
}