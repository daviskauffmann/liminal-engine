#include "sun.hpp"

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
}
