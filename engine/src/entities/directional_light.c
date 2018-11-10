#include <engine/engine.h>

struct directional_light *directional_light_create(
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular)
{
    struct directional_light *directional_light = malloc(sizeof(struct directional_light));

    if (!directional_light)
    {
        error("Couldn't allocate directional light");

        return NULL;
    }

    glm_vec_copy(direction, directional_light->direction);
    glm_vec_copy(ambient, directional_light->ambient);
    glm_vec_copy(diffuse, directional_light->diffuse);
    glm_vec_copy(specular, directional_light->specular);

    return directional_light;
}

void directional_light_calc_projection(struct directional_light *directional_light, vec4 *projection)
{
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, projection);
}

void directional_light_calc_view(struct directional_light *directional_light, vec4 *view)
{
    vec3 position;
    glm_vec_sub((vec3) { 0.0f, 0.0f, 0.0f }, directional_light->direction, position);

    glm_lookat(position, (vec3) { 0.0f, 0.0f, 0.0f }, (vec3) { 0.0f, 1.0f, 0.0f }, view);
}

void directional_light_destroy(struct directional_light *directional_light)
{
    free(directional_light);
}
