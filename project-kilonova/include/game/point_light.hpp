#ifndef GAME_POINT_LIGHT_H
#define GAME_POINT_LIGHT_H

struct point_light
{
    vec3 position;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 attenuation;
};

struct point_light *point_light_create(
    vec3 position,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color,
    vec3 attenuation);
void point_light_destroy(struct point_light *point_light);

#endif
