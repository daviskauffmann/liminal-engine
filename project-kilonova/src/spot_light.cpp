#include "spot_light.hpp"

namespace pk
{
spot_light::spot_light(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 color,
    float inner_cutoff,
    float outer_cutoff)
    : position(position),
      direction(direction),
      color(color),
      inner_cutoff(inner_cutoff),
      outer_cutoff(outer_cutoff)
{
}
} // namespace pk
