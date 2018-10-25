#include <engine/engine.h>

struct scene *scene_create(
    vec3 sun_direction,
    vec3 sun_ambient,
    vec3 sun_diffuse,
    vec3 sun_specular)
{
    struct scene *scene = malloc(sizeof(struct scene));

    if (!scene)
    {
        error("Couldn't allocate scene");

        return NULL;
    }

    glm_vec_copy(sun_direction, scene->sun_direction);
    glm_vec_copy(sun_ambient, scene->sun_ambient);
    glm_vec_copy(sun_diffuse, scene->sun_diffuse);
    glm_vec_copy(sun_specular, scene->sun_specular);

    return scene;
}

void scene_calc_sun_projection(struct scene *scene, vec4 *sun_projection)
{
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f, sun_projection);
}

void scene_calc_sun_view(struct scene *scene, vec4 *sun_view)
{
    vec3 sun_position;
    glm_vec_sub((vec3) { 0.0f, 0.0f, 0.0f }, scene->sun_direction, sun_position);

    glm_lookat(sun_position, (vec3) { 0.0f, 0.0f, 0.0f }, (vec3) { 0.0f, 1.0f, 0.0f }, sun_view);
}

void scene_destroy(struct scene *scene)
{
    free(scene);
}
