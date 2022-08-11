#ifndef LIMINAL_COMPONENTS_MESH_RENDERER_HPP
#define LIMINAL_COMPONENTS_MESH_RENDERER_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace liminal
{
    class model;

    class mesh_renderer
    {
    public:
        const liminal::model *model;
        glm::vec3 color;
        std::vector<glm::mat4> bone_transformations;

        mesh_renderer(const liminal::model *model = nullptr, const glm::vec3 &color = glm::vec3(1, 1, 1));
    };
}

#endif
