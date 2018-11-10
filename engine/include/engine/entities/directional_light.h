#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

struct directional_light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct directional_light ENGINE_API *directional_light_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular);
void ENGINE_API directional_light_calc_projection(struct directional_light *directional_light, vec4 *projection);
void ENGINE_API directional_light_calc_view(struct directional_light *directional_light, vec4 *view);
void ENGINE_API directional_light_destroy(struct directional_light *directional_light);

#endif
