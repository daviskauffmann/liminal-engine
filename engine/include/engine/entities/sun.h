#ifndef SUN_H
#define SUN_H

struct sun
{
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct sun ENGINE_API *sun_create(
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color);
void ENGINE_API sun_destroy(struct sun *sun);

#endif
