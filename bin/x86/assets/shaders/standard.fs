#version 330 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
    float glow;
} material;

out vec4 color;

void main()
{
    color = vec4(vec3(texture(material.diffuse, vertex.uv)), 1.0);
}
