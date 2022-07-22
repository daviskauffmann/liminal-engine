#version 460 core

#include "glsl/distribution_ggx.glsl"
#include "glsl/fresnel_schlick.glsl"
#include "glsl/geometry_smith.glsl"
#include "glsl/math.glsl"

in struct Vertex
{
    vec2 uv;
} vertex;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

uniform struct Camera
{
    vec3 position;
} camera;

uniform struct Geometry
{
    sampler2D position_map;
    sampler2D normal_map;
    sampler2D albedo_map;
    sampler2D material_map;
} geometry;

uniform struct Light
{
    vec3 direction;
    vec3 color;
    sampler2D depth_map;
    mat4 transformation_matrix;
} light;

void main()
{
    vec3 position = texture(geometry.position_map, vertex.uv).rgb;
    vec3 normal = texture(geometry.normal_map, vertex.uv).rgb;
    vec3 albedo = texture(geometry.albedo_map, vertex.uv).rgb;
    float metallic = texture(geometry.material_map, vertex.uv).r;
    float roughness = texture(geometry.material_map, vertex.uv).g;
    float ao = texture(geometry.material_map, vertex.uv).b;
    float height = texture(geometry.material_map, vertex.uv).a;

    vec3 n = normalize(normal);
    vec3 v = normalize(camera.position - position);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 l = normalize(-light.direction);
    vec3 h = normalize(v + l);
    vec3 radiance = light.color;

    float ndf = distribution_ggx(n, h, roughness);
    float g = geometry_smith(n, v, l, roughness);
    vec3 f = fresnel_schlick(clamp(dot(h, v), 0, 1), f0);

    vec3 numerator = ndf * g * f;
    float denominator = 4 * max(dot(n, v), 0) * max(dot(n, l), 0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 ks = f;
    vec3 kd = vec3(1) - ks;
    kd *= 1 - metallic;
    float n_dot_l = max(dot(n, l), 0);
    vec3 color = (kd * albedo / PI + specular) * radiance * n_dot_l * ao;
   
    vec4 light_space_position = light.transformation_matrix * vec4(position, 1);
    vec3 light_space_proj_coords = (light_space_position.xyz / light_space_position.w) * 0.5 + 0.5;
    float current_depth = light_space_proj_coords.z;
    float bias = max(0.005 * (1 - dot(n, l)), 0.005);
    float shadow = 0;
    vec2 texel_size = float(1) / textureSize(light.depth_map, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(light.depth_map, light_space_proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1 : 0;
        }
    }
    shadow /= 9;
    if (light_space_proj_coords.z > 1) shadow = 0;
    color = (1 - shadow) * color;

    frag_color = vec4(color, 1);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1)
    {
        bright_color = vec4(color, 1);
    }
    else
    {
        bright_color = vec4(0, 0, 0, 1);
    }
}
