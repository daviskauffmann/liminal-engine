#include "water.hpp"

#include <glm/gtc/matrix_transform.hpp>

liminal::water::water(glm::vec3 position, float size)
    : position(position), size(size)
{
}

glm::mat4 liminal::water::get_model_matrix() const
{
    glm::mat4 model = glm::identity<glm::mat4>();

    model = glm::translate(model, position);

    model = glm::scale(model, {size, 1.0f, size});

    return model;
}
