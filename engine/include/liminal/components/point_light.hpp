#ifndef LIMINAL_COMPONENTS_POINT_LIGHT_HPP
#define LIMINAL_COMPONENTS_POINT_LIGHT_HPP

#include <array>
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    namespace components
    {
        struct point_light
        {
            glm::vec3 color = {1, 1, 1};

            std::array<glm::mat4, 6> view_projection_matrices = {};
            GLuint depth_cubemap_id = 0;
        };
    }
}

#endif
