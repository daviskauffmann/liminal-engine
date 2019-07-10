#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

struct camera
{
    vec3 position;
    float pitch;
    float yaw;
    float roll;
    float fov;
};

struct camera ENGINE_API *camera_create(
    vec3 position,
    float pitch,
    float yaw,
    float roll,
    float fov);
void ENGINE_API camera_calc_front(struct camera *camera, vec3 *front);
void ENGINE_API camera_calc_up(struct camera *camera, vec3 *up);
void ENGINE_API camera_destroy(struct camera *camera);

#endif
