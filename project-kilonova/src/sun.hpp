#ifndef GAME_SUN_HPP
#define GAME_SUN_HPP

#include <glm/matrix.hpp>
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

        glm::mat4 calc_projection() const;
        glm::mat4 calc_view() const;
    };
}

#endif
