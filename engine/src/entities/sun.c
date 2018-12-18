#include <engine/engine.h>

struct sun *sun_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular)
{
    struct sun *sun = malloc(sizeof(struct sun));

    if (!sun)
    {
        printf("Error: Couldn't allocate sun\n");

        return NULL;
    }

    glm_vec_copy(direction, sun->direction);
    glm_vec_copy(ambient, sun->ambient);
    glm_vec_copy(diffuse, sun->diffuse);
    glm_vec_copy(specular, sun->specular);

    return sun;
}

void sun_destroy(struct sun *sun)
{
    free(sun);
}
