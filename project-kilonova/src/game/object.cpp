#include <game/game.h>

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
        printf("Error: Couldn't allocate object\n");

        return NULL;
    }

    object->mesh = mesh;
    object->material = material;
    glm_vec_copy(position, object->position);
    glm_vec_copy(rotation, object->rotation);
    glm_vec_copy(scale, object->scale);

    return object;
}

void object_calc_model(struct object *object, vec4 *model)
{
    glm_translate(model, object->position);
    glm_rotate(model, object->rotation[0], GLM_XUP);
    glm_rotate(model, object->rotation[1], GLM_YUP);
    glm_rotate(model, object->rotation[2], GLM_ZUP);
    glm_scale(model, object->scale);
}

void object_draw(
    struct object *object,
    unsigned int diffuse_map_index,
    unsigned int specular_map_index,
    unsigned int normal_map_index,
    unsigned int emission_map_index)
{
    if (object->material)
    {
        material_bind(object->material, diffuse_map_index, specular_map_index, normal_map_index, emission_map_index);
    }

    mesh_draw(object->mesh);
}

void object_destroy(struct object *object)
{
    free(object);
}
