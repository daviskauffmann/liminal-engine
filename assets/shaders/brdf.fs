#version 460 core

#include "glsl/geometry_smith.glsl"
#include "glsl/hammersley.glsl"
#include "glsl/importance_sample_ggx.glsl"

in struct Vertex
{
	vec2 uv;
} vertex;

out vec2 frag_color;

vec2 integrate_brdf(float n_dot_v, float roughness)
{
    vec3 v;
    v.x = sqrt(1.0 - n_dot_v * n_dot_v);
    v.y = 0.0;
    v.z = n_dot_v;

    float a = 0.0;
    float b = 0.0;
    vec3 n = vec3(0.0, 0.0, 1.0);
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 xi = hammersley(i, SAMPLE_COUNT);
        vec3 h  = importance_sample_ggx(xi, n, roughness);
        vec3 l  = normalize(2.0 * dot(v, h) * h - v);

        float n_dot_l = max(l.z, 0.0);
        float n_dot_h = max(h.z, 0.0);
        float v_dot_h = max(dot(v, h), 0.0);

        if (n_dot_l > 0.0)
        {
            float g = geometry_smith(n, v, l, roughness);
            float g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            float fc = pow(1.0 - v_dot_h, 5.0);

            a += (1.0 - fc) * g_vis;
            b += fc * g_vis;
        }
    }
    a /= float(SAMPLE_COUNT);
    b /= float(SAMPLE_COUNT);
    return vec2(a, b);
}

void main()
{
    frag_color = integrate_brdf(vertex.uv.x, vertex.uv.y);
}
