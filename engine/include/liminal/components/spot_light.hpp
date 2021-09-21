#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    struct spot_light
    {
        glm::vec3 color;
        float inner_cutoff;
        float outer_cutoff;
    };
}

#endif