#ifndef LIMINAL_COMPONENTS_MESH_RENDERER_HPP
#define LIMINAL_COMPONENTS_MESH_RENDERER_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class model;

    class mesh_renderer
    {
    public:
        liminal::model *model = nullptr;
        glm::vec3 color = {1, 1, 1};
    };
}

#endif
