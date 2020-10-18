#version 460 core

#include "glsl/fresnel_schlick.glsl"

in struct Vertex
{
    vec2 uv;
} vertex;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Geometry
{
    sampler2D position_map;
    sampler2D normal_map;
    sampler2D albedo_map;
    sampler2D material_map;
} geometry;

uniform struct Skybox
{
    samplerCube irradiance_cubemap;
    samplerCube prefilter_cubemap;
} skybox;

uniform sampler2D brdf_map;

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
    vec3 r = reflect(-v, n);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 f = fresnel_schlick_roughness(max(dot(n, v), 0.0), f0, roughness);
    vec3 ks = f;
    vec3 kd = 1.0 - ks;
    kd *= 1.0 - metallic;
    vec3 irradiance = texture(skybox.irradiance_cubemap, n).rgb;
    vec3 diffuse = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilter = textureLod(skybox.prefilter_cubemap, r,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdf_map, vec2(max(dot(n, v), 0.0), roughness)).rg;
    vec3 specular = prefilter * (f * brdf.x + brdf.y);
    vec3 color = (kd * diffuse + specular) * ao;

    frag_color = vec4(color, 1.0);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
    {
        bright_color = vec4(color, 1.0);
    }
    else
    {
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
