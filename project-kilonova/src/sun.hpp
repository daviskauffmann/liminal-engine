#ifndef GAME_SUN_HPP
#define GAME_SUN_HPP

#include <glm/vec3.hpp>

namespace pk
{
    struct sun
    {
        glm::vec3 direction;
        glm::vec3 ambient_color;
        glm::vec3 diffuse_color;
        glm::vec3 specular_color;

        sun(
            glm::vec3 direction,
            glm::vec3 ambient_color,
            glm::vec3 diffuse_color,
            glm::vec3 specular_color);
        ~sun();
    };
}

#endif
