#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "mesh.hpp"

namespace pk
{
    struct terrain
    {
        glm::vec3 position;
        pk::mesh *mesh;

        terrain(const std::string &filename, int grid_x, int grid_z);
        ~terrain();

        glm::mat4 calc_model() const;
    };
} // namespace pk

#endif
