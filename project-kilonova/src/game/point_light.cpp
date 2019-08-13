#include <game/game.h>

struct point_light *point_light_create(
    vec3 position,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color,
    vec3 attenuation)
{
    struct point_light *point_light = malloc(sizeof(struct point_light));

    if (!point_light)
    {
        printf("Error: Couldn't allocate point light\n");

        return NULL;
    }

    glm_vec_copy(position, point_light->position);
    glm_vec_copy(ambient_color, point_light->ambient_color);
    glm_vec_copy(diffuse_color, point_light->diffuse_color);
    glm_vec_copy(specular_color, point_light->specular_color);
    glm_vec_copy(attenuation, point_light->attenuation);

    return point_light;
}

void point_light_destroy(struct point_light *point_light)
{
    free(point_light);
}
