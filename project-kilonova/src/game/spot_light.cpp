#include <game/game.hpp>

namespace pk
{
    spot_light::spot_light(
        vec3 position,
        vec3 direction,
        vec3 ambient_color,
        vec3 diffuse_color,
        vec3 specular_color,
        vec3 attenuation,
        float inner_cutoff,
        float outer_cutoff)
    {
        glm_vec_copy(position, this->position);
        glm_vec_copy(direction, this->direction);
        glm_vec_copy(ambient_color, this->ambient_color);
        glm_vec_copy(diffuse_color, this->diffuse_color);
        glm_vec_copy(specular_color, this->specular_color);
        glm_vec_copy(attenuation, this->attenuation);
        this->inner_cutoff = inner_cutoff;
        this->outer_cutoff = outer_cutoff;
    }

    spot_light::~spot_light()
    {

    }
}
