#include "spot_light.hpp"

namespace pk
{
spot_light::spot_light(
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 ambient_color,
    glm::vec3 diffuse_color,
    glm::vec3 specular_color,
    glm::vec3 attenuation,
    float inner_cutoff,
    float outer_cutoff)
{
    this->position = position;
    this->direction = direction;
    this->ambient_color = ambient_color;
    this->diffuse_color = diffuse_color;
    this->specular_color = specular_color;
    this->attenuation = attenuation;
    this->inner_cutoff = inner_cutoff;
    this->outer_cutoff = outer_cutoff;
}

spot_light::~spot_light()
{
}
} // namespace pk
