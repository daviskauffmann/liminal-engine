#include <engine/engine.h>

struct material *material_create(
    vec3 color,
    struct texture *diffuse_map,
    struct texture *specular_map,
    float shininess,
    struct texture *normal_map,
    struct texture *emission_map,
    float glow)
{
    struct material *material = malloc(sizeof(struct material));

    if (!material)
    {
        printf("Error: Couldn't allocate material\n");

        return NULL;
    }

    glm_vec_copy(color, material->color);
    material->diffuse_map = diffuse_map;
    material->specular_map = specular_map;
    material->shininess = shininess;
    material->normal_map = normal_map;
    material->emission_map = emission_map;
    material->glow = glow;

    return material;
}

void material_bind(
    struct material *material,
    unsigned int diffuse_map_index,
    unsigned int specular_map_index,
    unsigned int normal_map_index,
    unsigned int emission_map_index)
{
    if (material->diffuse_map)
    {
        texture_bind(material->diffuse_map, diffuse_map_index);
    }

    if (material->specular_map)
    {
        texture_bind(material->specular_map, specular_map_index);
    }

    if (material->normal_map)
    {
        texture_bind(material->normal_map, normal_map_index);
    }

    if (material->emission_map)
    {
        texture_bind(material->emission_map, emission_map_index);
    }
}

void material_destroy(struct material *material)
{
    free(material);
}
