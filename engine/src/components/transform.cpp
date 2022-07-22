#include <liminal/components/transform.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

glm::mat4 liminal::transform::get_model_matrix() const
{
    auto model = parent ? parent->get_model_matrix() : glm::identity<glm::mat4>();
    return glm::translate(model, position) * glm::toMat4(glm::quat(rotation)) * glm::scale(model, scale);
}
