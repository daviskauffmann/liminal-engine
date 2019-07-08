#version 460 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Material
{
    vec3 color;
    sampler2D diffuse_map;
    sampler2D specular_map;
	float shininess;
    sampler2D normal_map;
    sampler2D emission_map;
	float glow;
} material;

layout (location = 0) out vec3 position_map;
layout (location = 1) out vec3 normal_map;
layout (location = 2) out vec4 albedo_specular_map;

void main()
{
    position_map = vertex.position;
    normal_map = normalize(vertex.normal);
    albedo_specular_map.rgb = texture(material.diffuse_map, vertex.uv).rgb * material.color;
    albedo_specular_map.a = texture(material.specular_map, vertex.uv).r;
}
