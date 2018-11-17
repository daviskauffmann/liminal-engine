#include <engine/engine.h>

struct point_light *point_light_create(
    vec3 position,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    vec3 attenuation)
{
    struct point_light *point_light = malloc(sizeof(struct point_light));

    if (!point_light)
    {
        printf("Error: Couldn't allocate point light\n");

        return NULL;
    }

    glm_vec_copy(position, point_light->position);
    glm_vec_copy(ambient, point_light->ambient);
    glm_vec_copy(diffuse, point_light->diffuse);
    glm_vec_copy(specular, point_light->specular);
    glm_vec_copy(attenuation, point_light->attenuation);

    return point_light;
}

void point_light_destroy(struct point_light *point_light)
{
    free(point_light);
}
