#include <malloc.h>

#include "error.h"
#include "material.h"

struct material *material_create(
    struct texture *diffuse,
    struct texture *specular,
    struct texture *emission,
    vec3 color,
    float shininess,
    float glow)
{
    struct material *material = malloc(sizeof(struct material));

    if (!material)
    {
        error_set("Couldn't allocate material");

        return NULL;
    }

    material->diffuse = diffuse;
    material->specular = specular;
    material->emission = emission;
    glm_vec_copy(color, material->color);
    material->shininess = shininess;
    material->glow = glow;

    return material;
}

void material_bind(struct material *material)
{
    if (material->diffuse)
        texture_bind(material->diffuse, 0);
    if (material->specular)
        texture_bind(material->specular, 1);
    if (material->emission)
        texture_bind(material->emission, 2);
}

void material_unbind(void)
{
    texture_unbind(0);
    texture_unbind(1);
    texture_unbind(2);
}

void material_destroy(struct material *material)
{
    free(material);
}
