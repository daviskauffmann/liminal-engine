#include <liminal/components/transform.hpp>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 liminal::transform::get_model_matrix() const
{
    glm::mat4 model = parent ? parent->get_model_matrix() : glm::identity<glm::mat4>();

    model = glm::translate(model, position);

    model = glm::rotate(model, rotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, rotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, rotation.z, glm::vec3(0, 0, 1));

    model = glm::scale(model, scale);

    return model;
}
