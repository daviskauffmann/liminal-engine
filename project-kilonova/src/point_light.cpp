#include "point_light.hpp"

namespace pk
{
    point_light::point_light(
        glm::vec3 position,
        glm::vec3 ambient_color,
        glm::vec3 diffuse_color,
        glm::vec3 specular_color,
        glm::vec3 attenuation)
    {
        this->position = position;
        this->ambient_color = ambient_color;
        this->diffuse_color = diffuse_color;
        this->specular_color = specular_color;
        this->attenuation = attenuation;
    }

    point_light::~point_light()
    {

    }
}
