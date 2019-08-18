#include "sun.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
    sun::sun(
        glm::vec3 direction,
        glm::vec3 ambient_color,
        glm::vec3 diffuse_color,
        glm::vec3 specular_color)
    {
        this->direction = direction;
        this->ambient_color = ambient_color;
        this->diffuse_color = diffuse_color;
        this->specular_color = specular_color;
    }

    sun::~sun()
    {

    }

    glm::mat4 sun::calc_projection() const
    {
        glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);

        return projection;
    }

    glm::mat4 sun::calc_view() const
    {
        glm::vec3 position = glm::vec3(0.0f) - this->direction;
        glm::mat4 view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
        return view;
    }
}
