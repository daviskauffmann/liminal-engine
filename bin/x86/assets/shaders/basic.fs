#version 330 core

in struct Vertex
{
    vec3 position;
    vec2 uv;
} vertex;

uniform int time;

uniform struct Material
{
    sampler2D diffuse;
    vec3 color;
} material;

void main()
{
    gl_FragColor = vec4(vec3(texture(material.diffuse, vertex.uv)), 1.0) * vec4(material.color, 1.0);
}
