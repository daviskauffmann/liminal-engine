#ifndef LIMINAL_COMPONENTS_CAMERA_HPP
#define LIMINAL_COMPONENTS_CAMERA_HPP

#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class transform;

    class camera
    {
    public:
        static constexpr float near_plane = 0.1f;
        static constexpr float far_plane = 1000;

        float fov;

        bool render_to_texture;
        mutable GLuint render_texture_id = 0;

        camera(float fov = 90, bool render_to_texture = false);

        glm::vec3 calc_front(const liminal::transform &transform) const;
        glm::vec3 calc_right(const liminal::transform &transform) const;
        glm::mat4 calc_projection(float aspect) const;
        glm::mat4 calc_view(const liminal::transform &transform) const;
    };
}

#endif
