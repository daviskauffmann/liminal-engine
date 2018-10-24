#ifndef MATERIAL_H
#define MATERIAL_H

#include <cglm/cglm.h>

#include "texture.h"

struct material
{
    struct texture *diffuse;
    struct texture *specular;
    struct texture *normal;
    struct texture *emission;
    vec3 color;
};

struct material ENGINE_API *material_create(
    struct texture *diffuse,
    struct texture *specular,
    struct texture *normal,
    struct texture *emission,
    vec3 color);
void ENGINE_API material_destroy(struct material *material);

#endif
