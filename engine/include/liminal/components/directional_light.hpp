#ifndef LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP
#define LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP

#include <array>
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class directional_light
    {
    public:
        glm::vec3 color = {1, 1, 1};
        bool shadows = true;

        glm::mat4 view_projection_matrix = {};
        GLuint depth_texture_id = 0;
    };
}

#endif
