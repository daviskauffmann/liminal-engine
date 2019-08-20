#include "directional_light.hpp"

namespace pk
{
directional_light::directional_light(
    glm::vec3 direction,
    glm::vec3 ambient_color,
    glm::vec3 diffuse_color,
    glm::vec3 specular_color)
    : direction(direction), ambient_color(ambient_color), diffuse_color(diffuse_color), specular_color(specular_color)
{
}

directional_light::~directional_light()
{
}
} // namespace pk
