#ifndef GAME_DIRECTIONAL_LIGHT_H
#define GAME_DIRECTIONAL_LIGHT_H

struct directional_light
{
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct directional_light *directional_light_create(
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color);
void directional_light_destroy(struct directional_light *directional_light);

#endif
