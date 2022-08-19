#ifndef LIMINAL_COMPONENTS_SPRITE_HPP
#define LIMINAL_COMPONENTS_SPRITE_HPP

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <liminal/graphics/texture.hpp>

namespace liminal
{
    struct sprite
    {
        liminal::texture *texture;
        glm::vec3 color = {1, 1, 1};
        glm::vec2 position = {};
        float rotation = 0;
        glm::vec2 scale = {1, 1};

        glm::mat4 get_model_matrix() const;
    };
}

#endif
