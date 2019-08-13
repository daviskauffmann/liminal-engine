#include <game/game.h>

struct directional_light *directional_light_create(
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color)
{
    struct directional_light *directional_light = malloc(sizeof(struct directional_light));

    if (!directional_light)
    {
        printf("Error: Couldn't allocate directional light\n");

        return NULL;
    }

    glm_vec_copy(direction, directional_light->direction);
    glm_vec_copy(ambient_color, directional_light->ambient_color);
    glm_vec_copy(diffuse_color, directional_light->diffuse_color);
    glm_vec_copy(specular_color, directional_light->specular_color);

    return directional_light;
}

void directional_light_destroy(struct directional_light *directional_light)
{
    free(directional_light);
}
