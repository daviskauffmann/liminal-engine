#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <GL/glew.h>

struct mesh;
struct material;

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
void object_update(struct object *object);
void object_draw(struct object *object);
void object_destroy(struct object *object);

#endif
