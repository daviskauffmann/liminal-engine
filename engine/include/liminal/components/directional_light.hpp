#ifndef LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP
#define LIMINAL_COMPONENTS_DIRECTIONAL_LIGHT_HPP

#include <glm/vec3.hpp>

namespace liminal
{
    class directional_light
    {
    public:
        glm::vec3 color = glm::vec3(1);
    };
}

#endif
