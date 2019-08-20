#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

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
    ~object();

    glm::mat4 calc_model() const;
    void draw(
        unsigned int diffuse_map_index,
        unsigned int specular_map_index,
        unsigned int normal_map_index,
        unsigned int emission_map_index) const;
};
} // namespace pk

#endif
