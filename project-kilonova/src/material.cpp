#include "material.hpp"

namespace pk
{
material::material(
    pk::texture *albedo_map,
    pk::texture *normal_map,
    pk::texture *metallic_map,
    pk::texture *roughness_map,
    pk::texture *ao_map)
    : albedo_map(albedo_map),
      normal_map(normal_map),
      metallic_map(metallic_map),
      roughness_map(roughness_map),
      ao_map(ao_map)
{
}
} // namespace pk
