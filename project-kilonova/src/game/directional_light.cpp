#include <game/game.hpp>

namespace pk
{
    directional_light::directional_light(
        vec3 direction,
        vec3 ambient_color,
        vec3 diffuse_color,
        vec3 specular_color)
    {
        glm_vec_copy(direction, this->direction);
        glm_vec_copy(ambient_color, this->ambient_color);
        glm_vec_copy(diffuse_color, this->diffuse_color);
        glm_vec_copy(specular_color, this->specular_color);
    }

    directional_light::~directional_light()
    {

    }
}
