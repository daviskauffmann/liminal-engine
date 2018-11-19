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

void sun_calc_projection(struct sun *sun, vec4 *projection)
{
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, projection);
}

void sun_calc_view(struct sun *sun, vec4 *view)
{
    vec3 position;
    glm_vec_sub((vec3) { 0.0f, 0.0f, 0.0f }, sun->direction, position);

    glm_lookat(position, (vec3) { 0.0f, 0.0f, 0.0f }, (vec3) { 0.0f, 1.0f, 0.0f }, view);
}

void sun_destroy(struct sun *sun)
{
    free(sun);
}
