#ifndef LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP
#define LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP

#include <GL/glew.h>
#include <array>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    class directional_light
    {
    public:
        static constexpr std::size_t num_cascades = 1;

        glm::vec3 color = glm::vec3(1);

        std::array<glm::mat4, num_cascades> view_projection_matrices = {};
        std::array<GLuint, num_cascades> depth_map_texture_ids = {};
    };
}

#endif
