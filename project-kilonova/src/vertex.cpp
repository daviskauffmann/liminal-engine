#include "vertex.hpp"

namespace pk
{
vertex::vertex(
    glm::vec3 position,
    glm::vec3 normal,
    glm::vec2 uv)
    : position(position),
      normal(normal),
      uv(uv)
{
}
} // namespace pk
