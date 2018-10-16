#include <malloc.h>
#include <SDL/SDL.h>

#include "error.h"
#include "object.h"
#include "time.h"

// TODO: scenes
// objects should be created inside a scene
// the game then can make a call to draw an entire scene, rather than specific objects
// also, rather than drawing a scene explicitly, the game can set a scene to be active, which the engine will draw each render pass
// the engine is responsible for sorting and reducing draw calls for all objects in the scene

struct object *object_create(
    struct mesh *mesh,
    struct material *material,
    vec3 position,
    vec3 rotation,
    vec3 scale)
{
    struct object *object = malloc(sizeof(struct object));

    if (!object)
    {
        error_set("Couldn't allocate object");

        return NULL;
    }

    object->mesh = mesh;
    object->material = material;
    glm_vec_copy(position, object->position);
    glm_vec_copy(rotation, object->rotation);
    glm_vec_copy(scale, object->scale);

    return object;
}

void object_update(struct object *object)
{
    float angle = time_current() * 0.001f;
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
