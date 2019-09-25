#ifndef GAME_TERRAIN_HPP
#define GAME_TERRAIN_HPP

#include <glm/vec2.hpp>

#include "material.hpp"
#include "mesh.hpp"

namespace pk
{
struct terrain
{
    glm::vec2 position;
    pk::mesh *mesh;
    pk::material *material;

    terrain(int grid_x, int grid_z, pk::material *material);
    ~terrain();
};
} // namespace pk

#endif
