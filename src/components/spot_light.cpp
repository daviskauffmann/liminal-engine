#include "spot_light.hpp"

liminal::spot_light::spot_light(
    glm::vec3 color,
    float inner_cutoff,
    float outer_cutoff)
    : color(color),
      inner_cutoff(inner_cutoff),
      outer_cutoff(outer_cutoff)
{
}
