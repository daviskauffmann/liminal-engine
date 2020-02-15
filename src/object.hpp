#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

#include "material.hpp"
#include "mesh.hpp"

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
