#include <engine/engine.h>

struct sun *sun_create(
    vec3 direction,
    vec3 ambient_color,
    vec3 diffuse_color,
    vec3 specular_color)
{
    struct sun *sun = malloc(sizeof(struct sun));

    if (!sun)
    {
        printf("Error: Couldn't allocate sun\n");

        return NULL;
    }

    glm_vec_copy(direction, sun->direction);
    glm_vec_copy(ambient_color, sun->ambient_color);
    glm_vec_copy(diffuse_color, sun->diffuse_color);
    glm_vec_copy(specular_color, sun->specular_color);

    return sun;
}

void sun_destroy(struct sun *sun)
{
    free(sun);
}
