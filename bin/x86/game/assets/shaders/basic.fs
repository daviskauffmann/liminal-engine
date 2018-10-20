#version 330 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Material
{
    sampler2D diffuse;
    vec3 color;
} material;

void main()
{
    vec3 color = vec3(texture(material.diffuse, vertex.uv)) * material.color;

    gl_FragColor = vec4(color, 1.0);
}
