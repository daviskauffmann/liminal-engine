#version 330 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Material
{
    vec3 color;
    sampler2D diffuse;
    sampler2D specular;
	float shininess;
    sampler2D normal;
    sampler2D emission;
	float glow;
} material;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedo_specular;

void main()
{
    position = vertex.position;
    normal = normalize(vertex.normal);
    albedo_specular.rgb = texture(material.diffuse, vertex.uv).rgb * material.color;
    albedo_specular.a = texture(material.specular, vertex.uv).r;
}
