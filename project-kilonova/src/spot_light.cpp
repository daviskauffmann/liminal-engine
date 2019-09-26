#include "spot_light.hpp"

namespace pk
{
spot_light::spot_light(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 color,
    float inner_cutoff,
    float outer_cutoff)
{
    this->position = position;
    this->direction = direction;
    this->color = color;
    this->inner_cutoff = inner_cutoff;
    this->outer_cutoff = outer_cutoff;
}

spot_light::~spot_light()
{
}
} // namespace pk
