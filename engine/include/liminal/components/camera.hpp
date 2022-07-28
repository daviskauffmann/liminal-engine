#ifndef LIMINAL_COMPONENTS_CAMERA_HPP
#define LIMINAL_COMPONENTS_CAMERA_HPP

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <liminal/components/transform.hpp>

namespace liminal
{
    class camera
    {
    public:
        static constexpr float near_plane = 0.1f;
        static constexpr float far_plane = 1000;

        float fov;

        bool render_to_texture;
        mutable GLuint render_texture_id;

        camera(const float fov = 90, const bool render_to_texture = false);

        glm::vec3 calc_front(const liminal::transform &transform) const;
        glm::vec3 calc_right(const liminal::transform &transform) const;
        glm::mat4 calc_projection(const float aspect) const;
        glm::mat4 calc_view(const liminal::transform &transform) const;
    };
}

#endif
