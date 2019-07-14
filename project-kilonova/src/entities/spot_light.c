#include <engine/engine.h>

struct spot_light *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color,
    vec3 attenuation,
    float inner_cutoff,
    float outer_cutoff)
{
    struct spot_light *spot_light = malloc(sizeof(struct spot_light));

    if (!spot_light)
    {
        printf("Error: Couldn't allocate spot light\n");

        return NULL;
    }

    glm_vec_copy(position, spot_light->position);
    glm_vec_copy(direction, spot_light->direction);
    glm_vec_copy(ambient_color, spot_light->ambient_color);
    glm_vec_copy(diffuse_color, spot_light->diffuse_color);
    glm_vec_copy(specular_color, spot_light->specular_color);
    glm_vec_copy(attenuation, spot_light->attenuation);
    spot_light->inner_cutoff = inner_cutoff;
    spot_light->outer_cutoff = outer_cutoff;

    return spot_light;
}

void spot_light_destroy(struct spot_light *spot_light)
{
    free(spot_light);
}
