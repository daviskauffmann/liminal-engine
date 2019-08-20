#ifndef GAME_POINT_LIGHT_HPP
#define GAME_POINT_LIGHT_HPP

#include <glm/vec3.hpp>

namespace pk
{
struct point_light
{
    glm::vec3 position;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    glm::vec3 attenuation;

    point_light(
        glm::vec3 position,
        glm::vec3 ambient_color,
        glm::vec3 diffuse_color,
        glm::vec3 specular_color,
        glm::vec3 attenuation);
    ~point_light();
};
} // namespace pk

#endif
