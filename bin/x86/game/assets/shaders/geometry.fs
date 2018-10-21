#version 330 core

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec4 albedospec;

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D emission;
    vec3 color;
    float shininess;
    float glow;
} material;

void main()
{
    position = vertex.position;
    normal = normalize(vertex.normal);
    albedospec.rgb = texture(material.diffuse, vertex.uv).rgb * material.color;
    albedospec.a = texture(material.specular, vertex.uv).r;
    // emission = texture(material.emission) * material.glow;
}
