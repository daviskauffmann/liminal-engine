#include <game/game.hpp>

namespace pk
{
    water::water(vec3 position, vec2 scale)
    {
        glm_vec_copy(position, this->position);
        glm_vec_copy(scale, this->scale);
    }

    water::~water()
    {

    }
}
