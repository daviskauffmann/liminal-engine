#ifndef GAME_CAMERA_HPP
#define GAME_CAMERA_HPP

namespace pk
{
    struct camera
    {
        vec3 position;
        float pitch;
        float yaw;
        float roll;
        float fov;

        camera(
            vec3 position,
            float pitch,
            float yaw,
            float roll,
            float fov);
        ~camera();

        void calc_front(vec3 *front) const;
        void calc_up(vec3 *up) const;
    };
}

#endif
