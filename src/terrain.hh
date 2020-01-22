#ifndef GAME_TERRAIN_HPP
#define GAME_TERRAIN_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "material.hh"
#include "mesh.hh"

namespace pk
{
struct terrain
{
    glm::vec3 position;
    pk::mesh *mesh;
    pk::material *material;

    terrain(int grid_x, int grid_z, pk::material *material);
    ~terrain();

    glm::mat4 calc_model() const;
};
} // namespace pk

#endif