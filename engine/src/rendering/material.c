#include <engine/engine.h>

struct material *material_create(
    vec3 color,
    struct texture *diffuse,
    struct texture *specular,
    float shininess,
    struct texture *normal,
    struct texture *emission,
    float glow)
{
    struct material *material = malloc(sizeof(struct material));

    if (!material)
    {
        error("Couldn't allocate material");

        return NULL;
    }

    glm_vec_copy(color, material->color);
    material->diffuse = diffuse;
    material->specular = specular;
    material->shininess = shininess;
    material->normal = normal;
    material->emission = emission;
    material->glow = glow;

    return material;
}

void material_destroy(struct material *material)
{
    free(material);
}
