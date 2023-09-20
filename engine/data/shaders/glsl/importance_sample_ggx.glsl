#ifndef IMPORTANCE_SAMPLE_GGX_GLSL
#define IMPORTANCE_SAMPLE_GGX_GLSL

#include "glsl/math.glsl"

vec3 importance_sample_ggx(vec2 xi, vec3 n, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2 * PI * xi.x;
    float cos_theta = sqrt((1 - xi.y) / (1 + (a * a - 1) * xi.y));
    float sin_theta = sqrt(1 - cos_theta * cos_theta);
	
    vec3 h;
    h.x = cos(phi) * sin_theta;
    h.y = sin(phi) * sin_theta;
    h.z = cos_theta;
	
    vec3 up = abs(n.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(up, n));
    vec3 bitangent = cross(n, tangent);
	
    vec3 sample_position = tangent * h.x + bitangent * h.y + n * h.z;
    return normalize(sample_position);
}

#endif
