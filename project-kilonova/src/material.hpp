#ifndef GAME_MATERIAL_HPP
#define GAME_MATERIAL_HPP

#include <glm/vec3.hpp>

#include "texture.hpp"

namespace pk
{
struct material
{
    pk::texture *albedo_map;
    glm::vec3 albedo_color;
    pk::texture *normal_map;
    pk::texture *metallic_map;
    pk::texture *roughness_map;
    pk::texture *occlusion_map;
    pk::texture *height_map;

    material(
        pk::texture *albedo_map,
        glm::vec3 albedo_color,
        pk::texture *normal_map,
        pk::texture *metallic_map,
        pk::texture *roughness_map,
        pk::texture *occlusion_map,
        pk::texture *height_map);
};
} // namespace pk

#endif
