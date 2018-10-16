#include <engine/directional_light.h>
#include <malloc.h>

struct directional_light *directional_light_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular)
{
    struct directional_light *directional_light = malloc(sizeof(struct directional_light));

    glm_vec_copy(direction, directional_light->direction);
    glm_vec_copy(ambient, directional_light->ambient);
    glm_vec_copy(diffuse, directional_light->diffuse);
    glm_vec_copy(specular, directional_light->specular);

    return directional_light;
}

void directional_light_destroy(struct directional_light *directional_light)
{
    free(directional_light);
}
