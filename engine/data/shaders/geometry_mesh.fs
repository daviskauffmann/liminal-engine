#version 460 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 color;
} vertex;

layout (location = 0) out vec3 position_map;
layout (location = 1) out vec3 normal_map;
layout (location = 2) out vec3 color_map;
layout (location = 3) out vec3 albedo_map;
layout (location = 4) out vec4 material_map;
layout (location = 5) out int id_map;

uniform struct Material
{
    vec3 color;

    sampler2D albedo_map;

    bool has_normal_map;
    sampler2D normal_map;

    bool has_metallic_map;
    sampler2D metallic_map;
    float metallic;

    bool has_roughness_map;
    sampler2D roughness_map;
    float roughness;

    bool has_occlusion_map;
    sampler2D occlusion_map;

    bool has_height_map;
    sampler2D height_map;
} material;

uniform int write_id;
uniform int id;

vec3 calc_normal()
{
    vec3 tangent_normal = texture(material.normal_map, vertex.uv).xyz * 2 - 1;

    vec3 q1 = dFdx(vertex.position);
    vec3 q2 = dFdy(vertex.position);
    vec2 st1 = dFdx(vertex.uv);
    vec2 st2 = dFdy(vertex.uv);

    vec3 n = normalize(vertex.normal);
    vec3 t = normalize(q1 * st2.t - q2 * st1.t);
    vec3 b = -normalize(cross(n, t));
    mat3 tbn = mat3(t, b, n);

    return normalize(tbn * tangent_normal);
}

void main()
{
    vec4 albedo = texture(material.albedo_map, vertex.uv);
    if (albedo.a < 0.5)
    {
        discard;
    }

    position_map = vertex.position;
    normal_map = material.has_normal_map ? calc_normal() : normalize(vertex.normal);
    color_map = vertex.color * material.color;
    albedo_map = albedo.rgb;
    material_map.r = material.has_metallic_map ? texture(material.metallic_map, vertex.uv).r : material.metallic;
    material_map.g = material.has_roughness_map ? texture(material.roughness_map, vertex.uv).r : material.roughness;
    material_map.b = material.has_occlusion_map ? texture(material.occlusion_map, vertex.uv).r : 1;
    material_map.a = material.has_height_map ? texture(material.height_map, vertex.uv).r : 0;
    if (write_id != 0)
    {
        id_map = id;
    }
}
