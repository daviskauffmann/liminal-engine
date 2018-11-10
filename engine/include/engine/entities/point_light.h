#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

struct point_light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct point_light ENGINE_API *point_light_create(
    vec3 position,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    vec3 attenuation);
void ENGINE_API point_light_destroy(struct point_light *point_light);

#endif
