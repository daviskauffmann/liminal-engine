#ifndef GAME_SPOT_LIGHT_HPP
#define GAME_SPOT_LIGHT_HPP

#include <glm/vec3.hpp>

namespace pk
{
struct spot_light
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float inner_cutoff;
    float outer_cutoff;

    spot_light(
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 color,
        float inner_cutoff,
        float outer_cutoff);
};
} // namespace pk

#endif
