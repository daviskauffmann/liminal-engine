#ifndef LIMINAL_COMPONENTS_POINT_LIGHT_HPP
#define LIMINAL_COMPONENTS_POINT_LIGHT_HPP

#include "../graphics/cubemap.hpp"
#include <array>
#include <gl/glew.h>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <memory>

namespace liminal
{
    class point_light
    {
    public:
        glm::vec3 color = {1, 1, 1};

        std::array<glm::mat4, 6> view_projection_matrices = {};
        std::weak_ptr<liminal::cubemap> depth_cubemap_texture;
    };
}

#endif
