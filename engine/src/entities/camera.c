#include <engine/engine.h>

struct camera *camera_create(
    vec3 position,
    float pitch,
    float yaw,
    float roll,
    float fov)
{
    struct camera *camera = malloc(sizeof(struct camera));

    if (!camera)
    {
        printf("Error: Couldn't allocate camera\n");

        return NULL;
    }

    glm_vec_copy(position, camera->position);
    camera->pitch = pitch;
    camera->yaw = yaw;
    camera->roll = roll;
    camera->fov = fov;

    return camera;
}

void camera_calc_front(struct camera *camera, vec3 *front)
{
    vec3 camera_front = {
        cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->pitch)),
        sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch)) };
    glm_normalize(camera_front);
    glm_vec_copy(camera_front, *front);
}

void camera_calc_up(struct camera *camera, vec3 *up)
{
    // TODO: use roll?
    vec3 camera_up = { 0.0f, 1.0f, 0.0f };
    glm_vec_copy(camera_up, *up);
}

void camera_destroy(struct camera *camera)
{
    free(camera);
}
