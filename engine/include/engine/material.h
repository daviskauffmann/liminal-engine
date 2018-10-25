#ifndef MATERIAL_H
#define MATERIAL_H

#include <cglm/cglm.h>

#include "texture.h"

struct material
{
    vec3 color;
    struct texture *diffuse;
    struct texture *specular;
    float shininess;
    struct texture *normal;
    struct texture *emission;
    float glow;
};

struct material ENGINE_API *material_create(
    vec3 color,
    struct texture *diffuse,
    struct texture *specular,
    float shininess,
    struct texture *normal,
    struct texture *emission,
    float glow);
void ENGINE_API material_destroy(struct material *material);

#endif
