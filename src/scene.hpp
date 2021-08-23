#ifndef SCENE_HPP
#define SCENE_HPP

#include "components/mesh_renderer.hpp"
#include "components/transform.hpp"
#include "entity.hpp"

#define MAX_ENTITIES 255

namespace liminal
{
    struct scene
    {
        liminal::entity entities[MAX_ENTITIES];
        liminal::transform transforms[MAX_ENTITIES];
        liminal::mesh_renderer mesh_renderers[MAX_ENTITIES];
    };
}

#endif
