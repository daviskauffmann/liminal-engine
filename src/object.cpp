#include "object.hpp"

#include <glm/gtc/matrix_transform.hpp>

pk::object::object(
    pk::model *model,
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale)
    : model(model),
      position(position),
      rotation(rotation),
      scale(scale)
{
}

glm::mat4 pk::object::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}
