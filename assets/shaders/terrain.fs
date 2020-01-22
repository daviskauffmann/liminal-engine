#version 460 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Material
{
    sampler2D albedo_map;
} material;

out vec4 frag_color;

void main()
{
    frag_color = texture(material.albedo_map, vertex.uv);
}
