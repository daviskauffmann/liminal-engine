#include <GL/glew.h>
#include <malloc.h>

#include "light.h"

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

struct point_light *point_light_create(
    vec3 position,
    vec3 ambient,
    vec3 diffuse,
    vec3 specular,
    float constant,
    float linear,
    float quadratic)
{
    struct point_light *point_light = malloc(sizeof(struct point_light));

    glm_vec_copy(position, point_light->position);
    glm_vec_copy(ambient, point_light->ambient);
    glm_vec_copy(diffuse, point_light->diffuse);
    glm_vec_copy(specular, point_light->specular);
    point_light->constant = constant;
    point_light->linear = linear;
    point_light->quadratic = quadratic;

    return point_light;
}

void point_light_destroy(struct point_light *point_light)
{
    free(point_light);
}

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
