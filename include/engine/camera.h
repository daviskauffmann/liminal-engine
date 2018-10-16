#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

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

struct camera *camera_create(
    vec3 position,
    vec3 front,
    vec3 up,
    float pitch,
    float yaw,
    float roll,
    float fov);
void camera_calc_projection(struct camera *camera, vec4 *projection);
void camera_calc_view(struct camera *camera, vec4 *view);
void camera_destroy(struct camera *camera);

#endif
