#ifndef LIGHT_H
#define LIGHT_H

#include <cglm/cglm.h>

struct directional_light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct directional_light *directional_light_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular);
void directional_light_destroy(struct directional_light *directional_light);

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

struct spot_light *spot_light_create(
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
void spot_light_destroy(struct spot_light *spot_light);

#endif
