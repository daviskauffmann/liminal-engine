#include <liminal/components/camera.hpp>

#include <glm/gtc/matrix_transform.hpp>

float liminal::camera::near_plane = 0.1f;
float liminal::camera::far_plane = 1000;

liminal::camera::camera(float fov, bool render_to_texture)
    : fov(fov),
      render_to_texture(render_to_texture)
{
}

glm::vec3 liminal::camera::calc_front(liminal::transform &transform) const
{
    glm::vec3 front(
        cosf(glm::radians(transform.rotation.y)) * cosf(glm::radians(transform.rotation.x)),
        sinf(glm::radians(transform.rotation.x)),
        sinf(glm::radians(transform.rotation.y)) * cosf(glm::radians(transform.rotation.x)));
    return glm::normalize(front);
}

glm::vec3 liminal::camera::calc_right(liminal::transform &transform) const
{
    glm::vec3 front = calc_front(transform);
    glm::vec3 right = glm::cross(front, glm::vec3(0, 1, 0));
    return glm::normalize(right);
}

glm::mat4 liminal::camera::calc_projection(float aspect) const
{
    glm::mat4 projection = glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
    return projection;
}

glm::mat4 liminal::camera::calc_view(liminal::transform &transform) const
{
    glm::vec3 front = calc_front(transform);
    glm::vec3 target = transform.position + front;

    glm::vec3 right = calc_right(transform);
    glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(transform.rotation.z), front);
    right = glm::vec3(rotation * glm::vec4(right, 1));

    glm::vec3 up = glm::cross(right, front);
    up = glm::normalize(up);

    glm::mat4 view = glm::lookAt(transform.position, target, up);

    return view;
}
