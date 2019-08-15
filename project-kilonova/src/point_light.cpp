#include "point_light.hpp"

namespace pk
{
    point_light::point_light(
        vec3 position,
        vec3 ambient_color,
        vec3 diffuse_color,
        vec3 specular_color,
        vec3 attenuation)
    {
        glm_vec_copy(position, this->position);
        glm_vec_copy(ambient_color, this->ambient_color);
        glm_vec_copy(diffuse_color, this->diffuse_color);
        glm_vec_copy(specular_color, this->specular_color);
        glm_vec_copy(attenuation, this->attenuation);
    }

    point_light::~point_light()
    {

    }
}
