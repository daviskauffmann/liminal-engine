#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pk
{
struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
} // namespace pk

#endif
