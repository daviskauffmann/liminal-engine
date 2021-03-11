#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    struct camera
    {
        static float near_plane;
        static float far_plane;

        glm::vec3 position;
        float pitch;
        float yaw;
        float roll;
        float fov;

        camera(
            glm::vec3 position,
            float pitch,
            float yaw,
            float roll,
            float fov);

        glm::vec3 calc_front() const;
        glm::vec3 calc_right() const;
        glm::mat4 calc_projection(float aspect) const;
        glm::mat4 calc_view() const;
    };
} // namespace liminal

#endif
