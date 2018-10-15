#include <malloc.h>
#include <SDL/SDL.h>

#include "material.h"
#include "mesh.h"
#include "object.h"
#include "time.h"

struct object *object_create(
    struct mesh *mesh,
    struct material *material,
    vec3 position,
    vec3 rotation,
    vec3 scale)
{
    struct object *object = calloc(1, sizeof(struct object));

    object->mesh = mesh;
    object->material = material;
    glm_vec_copy(position, object->position);
    glm_vec_copy(rotation, object->rotation);
    glm_vec_copy(scale, object->scale);

    return object;
}

void object_update(struct object *object)
{
    float angle = current_time * 0.001f;
    object->rotation[0] = angle;
    object->rotation[1] = angle;
    object->rotation[2] = angle;
}

void object_calc_model(struct object *object, vec4 *model)
{
    glm_translate(model, object->position);
    glm_rotate(model, object->rotation[0], GLM_XUP);
    glm_rotate(model, object->rotation[1], GLM_YUP);
    glm_rotate(model, object->rotation[2], GLM_ZUP);
    glm_scale(model, object->scale);
}

void object_draw(struct object *object)
{
    material_bind(object->material);

    mesh_draw(object->mesh);

    material_unbind();
}

void object_destroy(struct object *object)
{
    free(object);
}
