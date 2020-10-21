#version 330 core

// not working, dumb https://web.archive.org/web/20200523142333/http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates/
out vec4 FragColor;

in vec3 col;

#extension GL_OES_standard_derivatives : enable
float edgeFactor()
{
    vec3 d = fwidth(vBC);
    vec3 a3 = smoothstep(vec3(0.0), d * 1.5, vBC);
    return min(min(a3.x, a3.y), a3.z);
}

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, (1.0 - edgeFactor()) * 0.95);
}