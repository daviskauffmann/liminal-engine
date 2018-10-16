#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>

#include "material.h"
#include "mesh.h"

struct object
{
    struct mesh *mesh;
    struct material *material;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

struct object *object_create(
    struct mesh *mesh,
    struct material *material,
    vec3 position,
    vec3 rotation,
    vec3 scale);
void object_calc_model(struct object *object, vec4 *model);
void object_draw(struct object *object);
void object_destroy(struct object *object);

#endif
