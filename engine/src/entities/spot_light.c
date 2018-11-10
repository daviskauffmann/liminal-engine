#include <engine/engine.h>

struct spot_light *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    vec3 attenuation,
    float inner_cutoff,
    float outer_cutoff)
{
    struct spot_light *spot_light = malloc(sizeof(struct spot_light));

    if (!spot_light)
    {
        error("Couldn't allocate spot light");

        return NULL;
    }

    glm_vec_copy(position, spot_light->position);
    glm_vec_copy(direction, spot_light->direction);
    glm_vec_copy(ambient, spot_light->ambient);
    glm_vec_copy(diffuse, spot_light->diffuse);
    glm_vec_copy(specular, spot_light->specular);
    glm_vec_copy(attenuation, spot_light->attenuation);
    spot_light->inner_cutoff = inner_cutoff;
    spot_light->outer_cutoff = outer_cutoff;

    return spot_light;
}

void spot_light_destroy(struct spot_light *spot_light)
{
    free(spot_light);
}
