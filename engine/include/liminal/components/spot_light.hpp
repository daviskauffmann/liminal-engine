#ifndef LIMINAL_COMPONENTS_SPOT_LIGHT_HPP
#define LIMINAL_COMPONENTS_SPOT_LIGHT_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class spot_light
    {
    public:
        glm::vec3 color = glm::vec3(1.f);
        float inner_cutoff = 12.5f;
        float outer_cutoff = 15.f;

        glm::mat4 view_projection_matrix = {};
        GLuint depth_map_texture_id = 0;
    };
}

#endif
