#ifndef GAME_SPOT_LIGHT_HPP
#define GAME_SPOT_LIGHT_HPP

#include <glm/vec3.hpp>

namespace pk
{
struct spot_light
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
    glm::vec3 attenuation;
    float inner_cutoff;
    float outer_cutoff;

    spot_light(
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 ambient_color,
        glm::vec3 diffuse_color,
        glm::vec3 specular_color,
        glm::vec3 attenuation,
        float inner_cutoff,
        float outer_cutoff);
    ~spot_light();
};
} // namespace pk

#endif
