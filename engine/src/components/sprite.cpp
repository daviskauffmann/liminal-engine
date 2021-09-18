#include "components/sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

liminal::sprite::sprite(
    liminal::texture *texture,
    glm::vec3 color,
    glm::vec2 position,
    float rotation,
    glm::vec2 scale)
    : texture(texture),
      color(color),
      position(position),
      rotation(rotation),
      scale(scale)
{
}

glm::mat4 liminal::sprite::get_model_matrix() const
{
    glm::mat4 model = glm::identity<glm::mat4>();

    model = glm::translate(model, glm::vec3(position.x, position.y, 0.0f));
    model = glm::translate(model, glm::vec3(scale.x * 0.5f, scale.y * 0.5f, 0.0f));

    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(scale.x * -0.5f, scale.y * -0.5f, 0.0f));

    model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0f));

    return model;
}
