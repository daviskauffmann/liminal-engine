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
} material;

uniform struct Light
{
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float inner_cutoff;
    float outer_cutoff;
	mat4 projection;
	mat4 view;
    sampler2D depth_map;
    samplerCube depth_cubemap;
} light;

uniform float far_plane;

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

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
    float a = roughness * roughness;
    float a_sq = a * a;
    float n_dot_h = max(dot(n, h), 0.0);
    float n_dot_h_sq = n_dot_h * n_dot_h;

    float nom = a_sq;
    float denom = (n_dot_h_sq * (a_sq - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = n_dot_v;
    float denom = n_dot_v * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);
    float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);
    float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_theta, vec3 f0)
{
    return f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0);
}

void main()
{
    vec3 albedo = pow(texture(material.albedo_map, vertex.uv).rgb * material.albedo_color, vec3(2.2));
    float metallic = texture(material.metallic_map, vertex.uv).r;
    float roughness = texture(material.roughness_map, vertex.uv).r;
    float ao = texture(material.occlusion_map, vertex.uv).r;

    vec3 n = calc_normal();
    vec3 v = normalize(camera.position - vertex.position);

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, albedo, metallic);

    vec3 lo = vec3(0.0);

    vec3 l;
    if (light.type == 0)
    {
        l = normalize(-light.direction);
    }
    else if (light.type == 1)
    {
        l = normalize(light.position - vertex.position);
    }
    else if (light.type == 2)
    {
        l = normalize(light.position - vertex.position);
    }
    vec3 h = normalize(v + l);
    vec3 radiance;
    if (light.type == 0)
    {
        radiance = light.color;
    }
    else if (light.type == 1)
    {
        float distance = length(light.position - vertex.position);
        float attenuation = 1.0 / (distance * distance);
        radiance = light.color * attenuation;
    }
    else if (light.type == 2)
    {
        float distance = length(light.position - vertex.position);
        float attenuation = 1.0 / (distance * distance);
        vec3 light_direction = normalize(light.position - vertex.position);
        float theta = dot(light_direction, normalize(-light.direction));
        float epsilon = light.inner_cutoff - light.outer_cutoff;
        float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
        radiance = light.color * attenuation * intensity;
    }

    float ndf = distribution_ggx(n, h, roughness);
    float g = geometry_smith(n, v, l, roughness);
    vec3 f = fresnel_schlick(clamp(dot(h, v), 0.0, 1.0), f0);

    vec3 nominator = ndf * g * f;
    float denominator = 4 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;

    float n_dot_l = max(dot(n, l), 0.0);

    lo += (kd * albedo / PI + specular) * radiance * n_dot_l;

    // TODO: image based lighting
    vec3 ambient = vec3(0.0) * albedo * ao;

    vec3 color = ambient + lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));
    
    if (light.type == 0)
    {
        vec4 light_space_position = light.projection * light.view * vec4(vertex.position, 1.0);
        vec3 light_space_proj_coords = (light_space_position.xyz / light_space_position.w) * 0.5 + 0.5;
        float current_depth = light_space_proj_coords.z;
        float bias = max(0.05 * (1.0 - dot(n, l)), 0.005); 
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

        color = ambient + (1.0 - shadow) * color;
    }
    else if (light.type == 1)
    {
        vec3 frag_to_light = vertex.position - light.position;
        float current_depth = length(frag_to_light);
        float shadow = 0.0;
        float bias = 0.15;
        int samples = 20;
        float view_distance = length(camera.position - vertex.position);
        float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
        for (int i = 0; i < samples; i++)
        {
            vec3 grid_sampling_disk[20] = vec3[]
            (
               vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
               vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
               vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
               vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
               vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
            );
            float closest_depth = texture(light.depth_cubemap, frag_to_light + grid_sampling_disk[i] * disk_radius).r;
            closest_depth *= far_plane;
            if (current_depth - bias > closest_depth)
            {
                shadow += 1.0;
            }
        }
        shadow /= float(samples);

        color = ambient + (1.0 - shadow) * color;
    }

    frag_color = vec4(color, 1.0);
}
