#version 460 core

#include "glsl/distribution_ggx.glsl"
#include "glsl/fresnel_schlick.glsl"
#include "glsl/geometry_smith.glsl"
#include "glsl/math.glsl"

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
    sampler2D albedo_map;
    vec3 albedo_color;
    sampler2D normal_map;
    sampler2D metallic_map;
    sampler2D roughness_map;
    sampler2D occlusion_map;
    sampler2D height_map;
} material;

uniform struct Light
{
    vec3 position;
    vec3 color;
    samplerCube depth_cubemap;
} light;

uniform float far_plane;

out vec4 frag_color;

vec3 calc_normal()
{
    vec3 tangent_normal = texture(material.normal_map, vertex.uv).xyz * 2.0 - 1.0;

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
    vec3 albedo = texture(material.albedo_map, vertex.uv).rgb * material.albedo_color;
    float metallic = texture(material.metallic_map, vertex.uv).r;
    float roughness = texture(material.roughness_map, vertex.uv).r;
    float ao = texture(material.occlusion_map, vertex.uv).r;

    vec3 n = calc_normal();
    vec3 v = normalize(camera.position - vertex.position);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 l = normalize(light.position - vertex.position);
    vec3 h = normalize(v + l);
    float distance = length(light.position - vertex.position);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    float ndf = distribution_ggx(n, h, roughness);
    float g = geometry_smith(n, v, l, roughness);
    vec3 f = fresnel_schlick(clamp(dot(h, v), 0.0, 1.0), f0);

    vec3 numerator = ndf * g * f;
    float denominator = 4 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;
    float n_dot_l = max(dot(n, l), 0.0);
    vec3 color = (kd * albedo / PI + specular) * radiance * n_dot_l;
    
    vec3 frag_to_light = vertex.position - light.position;
    float current_depth = length(frag_to_light);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float view_distance = length(camera.position - vertex.position);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
    vec3 grid_sampling_disk[20] = vec3[]
    (
        vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
        vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
        vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
        vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
    );
    for (int i = 0; i < samples; i++)
    {
        float closest_depth = texture(light.depth_cubemap, frag_to_light + grid_sampling_disk[i] * disk_radius).r;
        closest_depth *= far_plane;
        if (current_depth - bias > closest_depth)
        {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    color = (1.0 - shadow) * color;

    frag_color = vec4(color, 1.0);
}
