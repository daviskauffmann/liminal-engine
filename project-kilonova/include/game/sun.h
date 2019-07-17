#ifndef GAME_SUN_H
#define GAME_SUN_H

struct sun
{
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct sun *sun_create(
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color);
void sun_destroy(struct sun *sun);

#endif
