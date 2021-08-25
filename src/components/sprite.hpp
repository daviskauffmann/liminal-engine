#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "texture.hpp"

namespace liminal
{
    struct sprite
    {
        liminal::texture *texture;
        glm::vec3 color;
        glm::vec2 position;
        float rotation;
        glm::vec2 scale;

        sprite(
            liminal::texture *texture,
            glm::vec3 color,
            glm::vec2 position,
            float rotation,
            glm::vec2 scale);

        glm::mat4 calc_model() const;
    };
}

#endif
