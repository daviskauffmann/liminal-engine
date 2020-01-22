#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "material.hh"
#include "mesh.hh"

namespace pk
{
struct object
{
    pk::mesh *mesh;
    pk::material *material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    object(
        pk::mesh *mesh,
        pk::material *material,
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale);

    glm::mat4 calc_model() const;
};
} // namespace pk

#endif
