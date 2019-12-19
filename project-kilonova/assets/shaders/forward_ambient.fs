#version 460 core

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

uniform float far_plane;

uniform samplerCube irradiance_cubemap;
uniform samplerCube prefilter_cubemap;
uniform sampler2D brdf_map;

out vec4 frag_color;

const float PI = 3.14159265359;

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

vec3 fresnel_schlick_roughness(float cos_theta, vec3 f0, float roughness)
{
    return f0 + (max(vec3(1.0 - roughness), f0) - f0) * pow(1.0 - cos_theta, 5.0);
}

void main()
{
    vec3 albedo = texture(material.albedo_map, vertex.uv).rgb * material.albedo_color;
    float metallic = texture(material.metallic_map, vertex.uv).r;
    float roughness = texture(material.roughness_map, vertex.uv).r;
    float ao = texture(material.occlusion_map, vertex.uv).r;

    vec3 n = calc_normal();
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
