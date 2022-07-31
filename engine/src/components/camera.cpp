#include <liminal/components/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <liminal/components/transform.hpp>

liminal::camera::camera(const float fov, const bool render_to_texture)
    : fov(fov),
      render_to_texture(render_to_texture)
{
}

glm::vec3 liminal::camera::calc_front(const liminal::transform &transform) const
{
    const auto front = glm::vec3(
        glm::cos(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x)),
        glm::sin(glm::radians(transform.rotation.x)),
        glm::sin(glm::radians(transform.rotation.y)) * glm::cos(glm::radians(transform.rotation.x)));
    return glm::normalize(front);
}

glm::vec3 liminal::camera::calc_right(const liminal::transform &transform) const
{
    const auto front = calc_front(transform);
    const auto right = glm::cross(front, {0, 1, 0});
    return glm::normalize(right);
}

glm::mat4 liminal::camera::calc_projection(const float aspect) const
{
    return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

glm::mat4 liminal::camera::calc_view(const liminal::transform &transform) const
{
    const auto front = calc_front(transform);
    const auto rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(transform.rotation.z), front);
    const auto right = calc_right(transform);
    const auto rotated_right = glm::vec3(rotation * glm::vec4(right, 1));
    const auto up = glm::normalize(glm::cross(rotated_right, front));
    return glm::lookAt(transform.position, transform.position + front, up);
}
