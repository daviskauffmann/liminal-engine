#ifndef SUN_H
#define SUN_H

struct sun
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct sun ENGINE_API *sun_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular);
void ENGINE_API sun_destroy(struct sun *sun);

#endif
