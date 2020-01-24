#version 460 core

#include "glsl/hammersley.glsl"
#include "glsl/importance_sample_ggx.glsl"
#include "glsl/math.glsl"

in struct Vertex
{
	vec3 position;
} vertex;

uniform samplerCube environment_cubemap;
uniform float roughness;

out vec4 frag_color;

float distribution_ggx(vec3 n, vec3 h, float roughness)
{
    float a = roughness * roughness;
    float a_sq = a * a;
    float n_dot_h = max(dot(n, h), 0.0);
    float n_dot_h_sq = n_dot_h * n_dot_h;

    float numerator = a_sq;
    float denominator = (n_dot_h_sq * (a_sq - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / max(denominator, 0.001);
}

void main()
{		
    vec3 n = normalize(vertex.position);
    vec3 r = n;
    vec3 v = r;

    const uint SAMPLE_COUNT = 1024u;
    float total_weight = 0.0;   
    vec3 prefilter = vec3(0.0);     
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 xi = hammersley(i, SAMPLE_COUNT);
        vec3 h  = importance_sample_ggx(xi, n, roughness);
        vec3 l  = normalize(2.0 * dot(v, h) * h - v);

        float n_dot_l = max(dot(n, l), 0.0);
        if (n_dot_l > 0.0)
        {
            float d = distribution_ggx(n, h, roughness);
            float n_dot_h = max(dot(n, h), 0.0);
            float h_dot_v = max(dot(h, v), 0.0);
            float pdf = d * n_dot_h / (4.0 * h_dot_v) + 0.0001; 

            float resolution = 4096.0;
            float sa_texel = 4.0 * PI / (6.0 * resolution * resolution);
            float sa_sample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mip_level = roughness == 0.0 ? 0.0 : 0.5 * log2(sa_sample / sa_texel); 

            prefilter += textureLod(environment_cubemap, l, mip_level).rgb * n_dot_l;
            total_weight += n_dot_l;
        }
    }
    prefilter = prefilter / total_weight;

    frag_color = vec4(prefilter, 1.0);
}
