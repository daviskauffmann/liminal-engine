#include "sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
sprite::sprite(
    glm::vec3 color,
    pk::texture *color_map,
    glm::vec2 position,
    float rotation,
    glm::vec2 scale)
    : color(color),
      color_map(color_map),
      position(position),
      rotation(rotation),
      scale(scale)
{
}

glm::mat4 sprite::calc_model() const
{
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(this->position.x, this->position.y, 0.0f));
    model = glm::translate(model, glm::vec3(this->scale.x * 0.5f, this->scale.y * 0.5f, 0.0f));
    model = glm::rotate(model, this->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(this->scale.x * -0.5f, this->scale.y * -0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(this->scale.x, this->scale.y, 1.0f));
    return model;
}
} // namespace pk
