#ifndef LIMINAL_COMPONENTS_MESH_RENDERER_HPP
#define LIMINAL_COMPONENTS_MESH_RENDERER_HPP

#include <liminal/graphics/model.hpp>

namespace liminal
{
    class mesh_renderer
    {
    public:
        liminal::model *model;

        mesh_renderer(liminal::model *model = nullptr);
    };
}

#endif
