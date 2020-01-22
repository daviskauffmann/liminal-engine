#include "material.hpp"

namespace pk
{
material::material(
    pk::texture *albedo_map,
    glm::vec3 albedo_color,
    pk::texture *normal_map,
    pk::texture *metallic_map,
    pk::texture *roughness_map,
    pk::texture *occlusion_map,
    pk::texture *height_map)
    : albedo_map(albedo_map),
      albedo_color(albedo_color),
      normal_map(normal_map),
      metallic_map(metallic_map),
      roughness_map(roughness_map),
      occlusion_map(occlusion_map),
      height_map(height_map)
{
}
} // namespace pk
