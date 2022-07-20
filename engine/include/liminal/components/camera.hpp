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
        static float near_plane;
        static float far_plane;

        float fov;

        bool render_to_texture;
        GLuint render_texture_id;

        camera(float fov, bool render_to_texture = false);

        glm::vec3 calc_front(liminal::transform &transform) const;
        glm::vec3 calc_right(liminal::transform &transform) const;
        glm::mat4 calc_projection(float aspect) const;
        glm::mat4 calc_view(liminal::transform &transform) const;
    };
}

#endif
