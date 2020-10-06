#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

pk::camera::camera(glm::vec3 position, float pitch, float yaw, float roll, float fov)
    : position(position),
      pitch(pitch),
      yaw(yaw),
      roll(roll),
      fov(fov)
{
}

glm::vec3 pk::camera::calc_front() const
{
    glm::vec3 front(
        cosf(glm::radians(yaw)) * cosf(glm::radians(pitch)),
        sinf(glm::radians(pitch)),
        sinf(glm::radians(yaw)) * cosf(glm::radians(pitch)));
    return glm::normalize(front);
}

glm::vec3 pk::camera::calc_right() const
{
    glm::vec3 front = calc_front();
    glm::vec3 right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    return glm::normalize(right);
}

glm::mat4 pk::camera::calc_projection(float aspect) const
{
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    return projection;
}

glm::mat4 pk::camera::calc_view() const
{
    glm::vec3 front = calc_front();
    glm::vec3 target = position + front;

    glm::vec3 right = calc_right();
    glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(roll), front);
    right = glm::vec3(rotation * glm::vec4(right, 1.0f));
    glm::vec3 up = glm::cross(right, front);
    up = glm::normalize(up);

    glm::mat4 view = glm::lookAt(position, target, up);

    return view;
}
