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
void ENGINE_API camera_destroy(struct camera *camera);

#endif
