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
    vec3 albedo_color;
    sampler2D normal_map;
    sampler2D metallic_map;
    sampler2D roughness_map;
    sampler2D occlusion_map;
} material;

layout (location = 0) out vec3 position_map;
layout (location = 1) out vec3 normal_map;

void main()
{
    position_map = vertex.position;
    normal_map = normalize(vertex.normal);
}
