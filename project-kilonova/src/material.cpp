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

material::~material()
{
}

void material::bind(
    unsigned int albedo_map_index,
    unsigned int normal_map_index,
    unsigned int metallic_map_index,
    unsigned int roughness_map_index,
    unsigned int ao_map_index) const
{
    if (this->albedo_map)
    {
        this->albedo_map->bind(albedo_map_index);
    }

    if (this->normal_map)
    {
        this->normal_map->bind(normal_map_index);
    }

    if (this->metallic_map)
    {
        this->metallic_map->bind(metallic_map_index);
    }

    if (this->roughness_map)
    {
        this->roughness_map->bind(roughness_map_index);
    }

    if (this->ao_map)
    {
        this->ao_map->bind(ao_map_index);
    }
}
} // namespace pk
