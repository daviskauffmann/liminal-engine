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
    vec3 direction;
    vec3 color;
    float inner_cutoff;
    float outer_cutoff;
	mat4 projection;
	mat4 view;
    sampler2D depth_map;
} light;

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

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 l = normalize(light.position - vertex.position);
    vec3 h = normalize(v + l);
    float distance = length(light.position - vertex.position);
    float attenuation = 1.0 / (distance * distance);
    vec3 light_direction = normalize(light.position - vertex.position);
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.inner_cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    vec3 radiance = light.color * attenuation * intensity;

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
    
    vec4 light_space_position = light.projection * light.view * vec4(vertex.position, 1.0);
    vec3 light_space_proj_coords = (light_space_position.xyz / light_space_position.w) * 0.5 + 0.5;
    float current_depth = light_space_proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(n, l)), 0.005) * 0.1;
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(light.depth_map, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(light.depth_map, light_space_proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (light_space_proj_coords.z > 1.0) shadow = 0.0;
    color = (1.0 - shadow) * color;

    frag_color = vec4(color, 1.0);
}
