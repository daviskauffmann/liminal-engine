#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
    camera::camera(glm::vec3 position, float pitch, float yaw, float roll, float fov)
        : position(position)
        , pitch(pitch)
        , yaw(yaw)
        , roll(roll)
        , fov(fov)
    {
    }

    camera::~camera()
    {
    }

    glm::vec3 camera::calc_front() const
    {
        glm::vec3 front(
            cosf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch)),
            sinf(glm::radians(this->pitch)),
            sinf(glm::radians(this->yaw)) * cosf(glm::radians(this->pitch)));

        return glm::normalize(front);
    }

    glm::vec3 camera::calc_up() const
    {
        glm::vec3 up(0.0f, 1.0f, 0.0f);

        return up;
    }

    glm::mat4 camera::calc_projection(float aspect) const
    {
        glm::mat4 projection = glm::perspective(glm::radians(this->fov), aspect, 0.01f, 100.0f);

        return projection;
    }

    glm::mat4 camera::calc_view() const
    {
        glm::vec3 front = this->calc_front();
        glm::vec3 target = this->position + front;
        glm::vec3 up = this->calc_up();
        glm::mat4 view = glm::lookAt(this->position, target, up);

        return view;
    }
}
