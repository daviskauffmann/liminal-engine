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
    vec3 attenuation;
    float inner_cutoff;
    float outer_cutoff;
};

struct spot_light ENGINE_API *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    vec3 attenuation,
    float inner_cutoff,
    float outer_cutoff);
void ENGINE_API spot_light_destroy(struct spot_light *spot_light);

#endif
