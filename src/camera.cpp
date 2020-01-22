#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
camera::camera(glm::vec3 position, float pitch, float yaw, float roll, float fov)
    : position(position),
      pitch(pitch),
      yaw(yaw),
      roll(roll),
      fov(fov)
{
}

glm::vec3 camera::calc_front() const
{
    glm::vec3 front(
        cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)),
        sinf(glm::radians(pitch)),
        sinf(glm::radians(yaw)) * cosf(glm::radians(pitch)));
    return glm::normalize(front);
}

glm::vec3 camera::calc_right() const
{
    glm::vec3 front = calc_front();
    glm::vec3 right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::normalize(right);
}

glm::vec3 camera::calc_up() const
{
    glm::vec3 front = calc_front();
    glm::vec3 right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 up = glm::cross(right, front);
    return glm::normalize(up);
}

glm::mat4 camera::calc_projection(float aspect) const
{
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    return projection;
}

glm::mat4 camera::calc_view() const
{
    glm::vec3 front = calc_front();
    glm::vec3 target = position + front;
    glm::vec3 up = calc_up();
    glm::mat4 view = glm::lookAt(position, target, up);
    return view;
}
} // namespace pk
