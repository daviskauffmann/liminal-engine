#ifndef GAME_DIRECTIONAL_LIGHT_HPP
#define GAME_DIRECTIONAL_LIGHT_HPP

#include <glm/vec3.hpp>

namespace pk
{
    struct directional_light
    {
        glm::vec3 direction;
        glm::vec3 ambient_color;
        glm::vec3 diffuse_color;
        glm::vec3 specular_color;

        directional_light(
            glm::vec3 direction,
            glm::vec3 ambient_color,
            glm::vec3 diffuse_color,
            glm::vec3 specular_color);
        ~directional_light();
    };
}

#endif
