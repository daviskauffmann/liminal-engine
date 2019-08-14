#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

struct Camera
{
    vec3 position;
    float pitch;
    float yaw;
    float roll;
    float fov;

    Camera(
        vec3 position,
        float pitch,
        float yaw,
        float roll,
        float fov);
    ~Camera();

    void calcFront(vec3 *front) const;
    void calcUp(vec3 *up) const;
};

#endif
