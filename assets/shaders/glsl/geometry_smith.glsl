#ifndef GEOMETRY_SMITH_GLSL
#define GEOMETRY_SMITH_GLSL

float geometry_schlick_ggx(float n_dot_v, float roughness)
{
    float r = roughness;
    float k = (r * r) / 2;

    float numerator = n_dot_v;
    float denominator = n_dot_v * (1 - k) + k;

    return numerator / max(denominator, 0.001);
}

float geometry_smith(vec3 n, vec3 v, vec3 l, float roughness)
{
    float n_dot_v = max(dot(n, v), 0);
    float n_dot_l = max(dot(n, l), 0);
    float ggx1 = geometry_schlick_ggx(n_dot_l, roughness);
    float ggx2 = geometry_schlick_ggx(n_dot_v, roughness);

    return ggx1 * ggx2;
}

#endif
