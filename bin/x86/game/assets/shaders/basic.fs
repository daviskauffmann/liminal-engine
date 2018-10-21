#version 330 core

uniform struct Material
{
    vec3 color;
} material;

void main()
{
    vec3 color = material.color;

    gl_FragColor = vec4(color, 1.0);
}
