#version 330 core
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform vec2 resolution;
uniform float time;

// shader from https://www.shadertoy.com/view/tssSzM and https://www.shadertoy.com/view/wddyRH, thanks!!
#define PI 3.14159265359

// ------ PARAMETERS ------
float crtOutIntensity = 1.1; // intensity of crt cell outline
float crtInIntensity = 0.9; // intensity of crt cell inside
float aberrationIntensity = 1.5; // Intensity of chromatic aberration
int monochromeAberrations = 0;
float grainIntensity = 0.5; // Intensity of film grain
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

// CRT cells
vec3 crt(vec2 xy){
  xy=floor(xy*vec2(1.0,0.5));
  xy.x += xy.y*3.0;
  vec3 c = vec3(crtOutIntensity,crtOutIntensity,crtOutIntensity);
  xy.x = fract(xy.x/6.0);
    
  if(xy.x < 0.333)
      c.r=crtInIntensity;
  else if(xy.x < 0.666)
      c.g=crtInIntensity;
  else 
      c.b=crtInIntensity;
  return c;
}    

// from rez in Glenz vector form Hell
float rand(in vec2 p,in float t) {
	return fract(sin(dot(p+mod(t,1.0),vec2(12.9898,78.2333)))*43758.5453);
}

// Film grain
float grain(vec2 uv) {
    return 1.0-grainIntensity+grainIntensity*rand(uv,time/100);
}

void main()
{
    vec2 p = texCoords;
    //p = 2.*p -1.;
    //p *= 1. + pow(abs(p.yx)/vec2(5., 4.), vec2(2.));
    //p = .5*p+.5;
    vec3 col = aberration(texCoords);// * crt(texCoords);
    //col *= 2.;

    // line
    //col *= .4 + .7*(clamp(.3 + .3*sin(p.y * resolution.x * 1.5), 0., 1.));
    float vig = 16.*texCoords.x * texCoords.y * (1.-texCoords.x)*(1.-texCoords.y);
	col *= pow(vig, .2);
    

    FragColor = vec4(col, 1.0);// * grain(texCoords) * 0.8;
}