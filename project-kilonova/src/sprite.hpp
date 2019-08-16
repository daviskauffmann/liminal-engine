#ifndef GAME_SPRITE_HPP
#define GAME_SPRITE_HPP

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "texture.hpp"

namespace pk
{
    struct sprite
    {
        glm::vec3 color;
        pk::texture *color_map;
        glm::vec2 position;
        float rotation;
        glm::vec2 scale;

        sprite(
            glm::vec3 color,
            pk::texture *colorMap,
            glm::vec2 position,
            float rotation,
            glm::vec2 scale);
        ~sprite();

        glm::mat4 calc_model() const;
    };
}

#endif
