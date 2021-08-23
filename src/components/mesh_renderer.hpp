#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include "../component.hpp"
#include "../model.hpp"

namespace liminal
{
    struct mesh_renderer : public component
    {
        liminal::model *model;
    };
}

#endif
