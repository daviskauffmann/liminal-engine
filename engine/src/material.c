#include <engine/engine.h>

struct material *material_create(
    struct texture *diffuse,
    struct texture *specular,
    struct texture *normal,
    struct texture *emission,
    vec3 color)
{
    struct material *material = malloc(sizeof(struct material));

    if (!material)
    {
        error("Couldn't allocate material");

        return NULL;
    }

    material->diffuse = diffuse;
    material->specular = specular;
    material->normal = normal;
    material->emission = emission;
    glm_vec_copy(color, material->color);

    return material;
}

void material_destroy(struct material *material)
{
    free(material);
}
