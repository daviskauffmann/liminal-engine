#include <engine/engine.h>

struct spot_light *spot_light_create(
    vec3 position,
    vec3 direction,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    float constant,
    float linear,
    float quadratic,
    float cutOff,
    float outerCutOff)
{
    struct spot_light *spot_light = malloc(sizeof(struct spot_light));

    if (!spot_light)
    {
        error_set("Couldn't allocate spot light");

        return NULL;
    }

    glm_vec_copy(position, spot_light->position);
    glm_vec_copy(direction, spot_light->direction);
    glm_vec_copy(ambient, spot_light->ambient);
    glm_vec_copy(diffuse, spot_light->diffuse);
    glm_vec_copy(specular, spot_light->specular);
    spot_light->constant = constant;
    spot_light->linear = linear;
    spot_light->quadratic = quadratic;
    spot_light->cutOff = cutOff;
    spot_light->outerCutOff = outerCutOff;

    return spot_light;
}

void spot_light_destroy(struct spot_light *spot_light)
{
    free(spot_light);
}
