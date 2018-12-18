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
        printf("Error: Couldn't allocate camera\n");

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

void camera_destroy(struct camera *camera)
{
    free(camera);
}
