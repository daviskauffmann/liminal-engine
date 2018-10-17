#include <engine/engine.h>

struct camera *camera_create(
    vec3 position,
    vec3 front,
    vec3 up,
    float pitch,
    float yaw,
    float roll,
    float fov)
{
    struct camera *camera = malloc(sizeof(struct camera));

    if (!camera)
    {
        error("Couldn't allocate camera");

        return NULL;
    }

    glm_vec_copy(position, camera->position);
    glm_vec_copy(front, camera->front);
    glm_vec_copy(up, camera->up);
    camera->pitch = pitch;
    camera->yaw = yaw;
    camera->roll = roll;
    camera->fov = fov;

    return camera;
}

void camera_calc_projection(struct camera *camera, float aspect, vec4 *projection)
{
    glm_perspective(
        glm_rad(camera->fov),
        aspect,
        0.01f,
        100.0f,
        projection);
}

void camera_calc_view(struct camera *camera, vec4 *view)
{
    vec3 camera_target;
    glm_vec_add(camera->position, camera->front, camera_target);
    glm_lookat(
        camera->position,
        camera_target,
        camera->up,
        view);
}

void camera_destroy(struct camera *camera)
{
    free(camera);
}
