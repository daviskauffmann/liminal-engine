#ifndef MATERIAL_H
#define MATERIAL_H

#include <cglm/cglm.h>

#include "texture.h"

struct material
{
    struct texture *diffuse;
    struct texture *specular;
    struct texture *emission;
    vec3 color;
    float shininess;
    float glow;
};

struct material *material_create(
    struct texture *diffuse,
    struct texture *specular,
    struct texture *emission,
    vec3 color,
    float shininess,
    float glow);
void material_bind(struct material *material);
void material_unbind(void);
void material_destroy(struct material *material);

#endif
