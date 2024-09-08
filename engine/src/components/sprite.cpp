#include <liminal/components/sprite.hpp>

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 liminal::components::sprite::get_model_matrix() const
{
    // TODO: parenting?
    // this whole thing should just be removed, and 2d sprites should use the normal transform component
    auto model = glm::identity<glm::mat4>();

    model = glm::translate(model, {position.x, position.y, 0});
    model = glm::translate(model, {scale.x * 0.5f, scale.y * 0.5f, 0});

    model = glm::rotate(model, rotation, {0, 0, 1});
    model = glm::translate(model, {scale.x * -0.5f, scale.y * -0.5f, 0});

    model = glm::scale(model, {scale.x, scale.y, 1});

    return model;
}
