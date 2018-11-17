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

void camera_calc_projection_perspective(struct camera *camera, float aspect, vec4 *projection)
{
    glm_perspective(
        glm_rad(camera->fov),
        aspect,
        0.01f,
        100.0f,
        projection);
}

void camera_calc_projection_ortho(struct camera *camera, float aspect, vec4 *projection)
{
    glm_ortho_default(aspect, projection);
}

void camera_calc_view(struct camera *camera, vec4 *view)
{
    vec3 target;
    glm_vec_add(camera->position, camera->front, target);

    glm_lookat(
        camera->position,
        target,
        camera->up,
        view);
}

void camera_destroy(struct camera *camera)
{
    free(camera);
}
