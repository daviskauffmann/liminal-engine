#ifndef SCENE_H
#define SCENE_H

#include <cglm/cglm.h>

struct scene
{
    vec3 sun_direction;
    vec3 sun_ambient;
    vec3 sun_specular;
    vec3 sun_diffuse;
};

struct scene ENGINE_API *scene_create(
    vec3 sun_direction,
    vec3 sun_ambient,
    vec3 sun_diffuse,
    vec3 sun_specular); 
void ENGINE_API scene_calc_sun_projection(struct scene *scene, vec4 *sun_projection);
void ENGINE_API scene_calc_sun_view(struct scene *scene, vec4 *sun_view);
void ENGINE_API scene_destroy(struct scene *scene);

#endif
