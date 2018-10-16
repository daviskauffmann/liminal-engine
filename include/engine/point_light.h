#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <cglm/cglm.h>

struct point_light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct point_light *point_light_create(
    vec3 position,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    float constant,
    float linear,
    float quadratic);
void point_light_destroy(struct point_light *point_light);

#endif
