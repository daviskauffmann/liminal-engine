#ifndef CAMERA_H
#define CAMERA_H

// TODO: perhaps separate into different ortho and perspective camera structs?

struct camera
{
    vec3 position;
    vec3 front;
    vec3 up;
    float pitch;
    float yaw;
    float roll;
    float fov;
};

struct camera ENGINE_API *camera_create(
    vec3 position,
    vec3 front,
    vec3 up,
    float pitch,
    float yaw,
    float roll,
    float fov);
void ENGINE_API camera_calc_projection_perspective(struct camera *camera, float aspect, vec4 *projection);
void ENGINE_API camera_calc_projection_ortho(struct camera *camera, float aspect, vec4 *projection);
void ENGINE_API camera_calc_view(struct camera *camera, vec4 *view);
void ENGINE_API camera_destroy(struct camera *camera);

#endif
