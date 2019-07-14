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

struct camera *camera_create(
    vec3 position,
    float pitch,
    float yaw,
    float roll,
    float fov);
void camera_calc_front(struct camera *camera, vec3 *front);
void camera_calc_up(struct camera *camera, vec3 *up);
void camera_destroy(struct camera *camera);

#endif
