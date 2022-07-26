#ifndef LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP
#define LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class directional_light
    {
    public:
        glm::vec3 color = glm::vec3(1);

        glm::mat4 view_projection_matrix = {};
        GLuint depth_map_texture_id = 0;
    };
}

#endif
