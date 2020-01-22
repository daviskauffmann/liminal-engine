#include "object.hh"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
object::object(
    pk::mesh *mesh,
    pk::material *material,
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale)
    : mesh(mesh),
      material(material),
      position(position),
      rotation(rotation),
      scale(scale)
{
}

glm::mat4 object::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}
} // namespace pk
