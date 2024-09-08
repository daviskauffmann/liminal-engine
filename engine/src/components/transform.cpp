#include <liminal/components/transform.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

glm::mat4 liminal::components::transform::get_model_matrix() const
{
    const auto model = parent ? parent->get_model_matrix() : glm::identity<glm::mat4>();
    return glm::translate(model, position) * glm::toMat4(glm::quat(rotation)) * glm::scale(model, scale);
}

void liminal::components::transform::set_model_matrix(const glm::mat4 &matrix)
{
    glm::quat rotation_quat;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(matrix, scale, rotation_quat, position, skew, perspective);
    rotation = glm::eulerAngles(rotation_quat);
}
