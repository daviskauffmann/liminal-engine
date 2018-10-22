#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include <cglm/cglm.h>

struct spot_light
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

struct spot_light ENGINE_API *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    float constant,
    float linear,
    float quadratic,
    float cutOff,
    float outerCutOff);
void ENGINE_API spot_light_destroy(struct spot_light *spot_light);

#endif
