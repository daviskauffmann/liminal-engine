#ifndef DISTRIBUTION_GGL_GLSL
#define DISTRIBUTION_GGL_GLSL

#include "glsl/math.glsl"

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

#endif
