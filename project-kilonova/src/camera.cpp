#include "camera.hpp"

namespace pk
{
    camera::camera(
        vec3 position,
        float pitch,
        float yaw,
        float roll,
        float fov)
    {
        glm_vec_copy(position, this->position);
        this->pitch = pitch;
        this->yaw = yaw;
        this->roll = roll;
        this->fov = fov;
    }

    camera::~camera()
    {

    }

    void camera::calc_front(vec3 *front) const
    {
        vec3 camera_front = {
            cosf(glm_rad(this->yaw)) * cosf(glm_rad(this->pitch)),
            sinf(glm_rad(this->pitch)),
            sinf(glm_rad(this->yaw)) * cosf(glm_rad(this->pitch)) };
        glm_normalize(camera_front);
        glm_vec_copy(camera_front, *front);
    }

    void camera::calc_up(vec3 *up) const
    {
        vec3 camera_up = { 0.0f, 1.0f, 0.0f };
        glm_vec_copy(camera_up, *up);
    }
}
