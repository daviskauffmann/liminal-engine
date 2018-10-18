#include <engine/engine.h>

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
        error("Couldn't allocate object");

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

void object_draw(struct object *object, struct program *program)
{
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    object_calc_model(object, model);

    program_bind(program);

    program_set_mat4(program_get_location(program, "object.model"), model);
    program_set_vec3(program_get_location(program, "material.color"), object->material->color);
    program_set_float(program_get_location(program, "material.shininess"), object->material->shininess);
    program_set_float(program_get_location(program, "material.glow"), object->material->glow);

    material_bind(object->material);

    mesh_draw(object->mesh);

    material_unbind();
    program_unbind();
}

void object_destroy(struct object *object)
{
    free(object);
}
