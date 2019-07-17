#ifndef GAME_SPOT_LIGHT_H
#define GAME_SPOT_LIGHT_H

struct spot_light
{
    vec3 position;
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    vec3 attenuation;
    float inner_cutoff;
    float outer_cutoff;
};

struct spot_light *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color,
    vec3 attenuation,
    float inner_cutoff,
    float outer_cutoff);
void spot_light_destroy(struct spot_light *spot_light);

#endif
