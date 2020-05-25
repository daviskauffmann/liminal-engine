#version 460 core

#include "glsl/fresnel_schlick.glsl"
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

// TODO: material array, plus a blend map
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

uniform float far_plane;

uniform samplerCube irradiance_cubemap;
uniform samplerCube prefilter_cubemap;
uniform sampler2D brdf_map;

out vec4 frag_color;

const float height_scale = 1.0;

mat3 calc_tbn()
{
    vec3 q1 = dFdx(vertex.position);
    vec3 q2 = dFdy(vertex.position);
    vec2 st1 = dFdx(vertex.uv);
    vec2 st2 = dFdy(vertex.uv);
    vec3 n = normalize(vertex.normal);
    vec3 t = normalize(q1 * st2.t - q2 * st1.t);
    vec3 b = -normalize(cross(n, t));
    return mat3(t, b, n);
}

vec2 calc_parallax(mat3 tbn)
{
    float height = texture(material.height_map, vertex.uv).r;    
    vec3 v = normalize((tbn * camera.position) - (tbn * vertex.position));
    vec2 p = v.xy / v.z * (height * height_scale);
    return vertex.uv;   
}

vec3 calc_normal(mat3 tbn)
{
    vec3 tangent_normal = texture(material.normal_map, vertex.uv).xyz * 2.0 - 1.0;
    return normalize(tbn * tangent_normal);
}

void main()
{
    mat3 tbn = calc_tbn();
    vec2 uv = calc_parallax(tbn);
    // if(uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0)
    //     discard;

    vec3 albedo = texture(material.albedo_map, uv).rgb * material.albedo_color;
    float metallic = texture(material.metallic_map, uv).r;
    float roughness = texture(material.roughness_map, uv).r;
    float ao = texture(material.occlusion_map, uv).r;

    vec3 n = calc_normal(tbn);
    vec3 v = normalize(camera.position - vertex.position);
    vec3 r = reflect(-v, n);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 f = fresnel_schlick_roughness(max(dot(n, v), 0.0), f0, roughness);
    vec3 ks = f;
    vec3 kd = 1.0 - ks;
    kd *= 1.0 - metallic;
    vec3 irradiance = texture(irradiance_cubemap, n).rgb;
    vec3 diffuse = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilter = textureLod(prefilter_cubemap, r,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdf_map, vec2(max(dot(n, v), 0.0), roughness)).rg;
    vec3 specular = prefilter * (f * brdf.x + brdf.y);
    vec3 color = (kd * diffuse + specular) * ao;

    frag_color = vec4(color, 1.0);
}
