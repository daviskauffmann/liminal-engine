#ifndef COMPONENT_DIRECTIONAL_LIGHT_HPP
#define COMPONENT_DIRECTIONAL_LIGHT_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    struct directional_light
    {
        glm::vec3 color;

        directional_light(glm::vec3 color);
    };
}

#endif
