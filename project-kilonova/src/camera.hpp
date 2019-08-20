#ifndef GAME_CAMERA_HPP
#define GAME_CAMERA_HPP

#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

namespace pk
{
struct camera
{
    glm::vec3 position;
    float pitch;
    float yaw;
    float roll;
    float fov;

    camera(glm::vec3 position, float pitch, float yaw, float roll, float fov);
    ~camera();

    glm::vec3 calc_front() const;
    glm::vec3 calc_up() const;
    glm::mat4 calc_projection(float aspect) const;
    glm::mat4 calc_view() const;
};
} // namespace pk

#endif
