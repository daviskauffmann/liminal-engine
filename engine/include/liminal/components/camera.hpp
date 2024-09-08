#ifndef LIMINAL_COMPONENTS_CAMERA_HPP
#define LIMINAL_COMPONENTS_CAMERA_HPP

#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    namespace components
    {
        struct transform;

        struct camera
        {
            static constexpr float near_plane = 0.1f;
            static constexpr float far_plane = 1000;

            float fov;

            camera(float fov = 90);

            glm::vec3 calc_front(const liminal::components::transform &transform) const;
            glm::vec3 calc_right(const liminal::components::transform &transform) const;
            glm::mat4 calc_projection(float aspect) const;
            glm::mat4 calc_view(const liminal::components::transform &transform) const;
        };
    }
}

#endif
