#ifndef PK_VERTEX_HPP
#define PK_VERTEX_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pk
{
struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;

    vertex(
        glm::vec3 position,
        glm::vec3 normal,
        glm::vec2 uv);
};
} // namespace pk

#endif
