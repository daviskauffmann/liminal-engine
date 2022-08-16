#ifndef LIMINAL_COMPONENTS_SPOT_LIGHT_HPP
#define LIMINAL_COMPONENTS_SPOT_LIGHT_HPP

#include "../graphics/texture.hpp"
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    class spot_light
    {
    public:
        glm::vec3 color = {1, 1, 1};
        float inner_cutoff = 12.5f;
        float outer_cutoff = 15;

        glm::mat4 view_projection_matrix = {};
        std::weak_ptr<liminal::texture> depth_map_texture;
    };
}

#endif
