#version 330 core

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

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 albedo;
layout (location = 3) out vec3 specular;

void main()
{
    position = vertex.position;
    normal = normalize(vertex.normal); // TODO: normal map
    albedo = texture(material.diffuse, vertex.uv).rgb * material.color;
    specular = texture(material.specular, vertex.uv).rgb;
    // emission = texture(material.emission) * material.glow;
}
