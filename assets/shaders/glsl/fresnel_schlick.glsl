#ifndef FRESNEL_SCHLICK_GLSL
#define FRESNEL_SCHLICK_GLSL

vec3 fresnel_schlick(float cos_theta, vec3 f0)
{
    return f0 + (1 - f0) * pow(1 - cos_theta, 5);
}

vec3 fresnel_schlick_roughness(float cos_theta, vec3 f0, float roughness)
{
    return f0 + (max(vec3(1 - roughness), f0) - f0) * pow(1 - cos_theta, 5);
}

#endif
