#ifndef LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP
#define LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP

#include "../graphics/texture.hpp"
#include <array>
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    class directional_light
    {
    public:
        static constexpr std::size_t num_cascades = 1;

        glm::vec3 color = {1, 1, 1};
        bool shadows = true;

        std::array<glm::mat4, num_cascades> view_projection_matrices = {};
        std::array<std::weak_ptr<liminal::texture>, num_cascades> depth_map_textures;
    };
}

#endif
