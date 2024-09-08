#ifndef LIMINAL_COMPONENTS_SPRITE_HPP
#define LIMINAL_COMPONENTS_SPRITE_HPP

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace liminal
{
    namespace graphics
    {
        class texture;
    }

    namespace components
    {
        struct sprite
        {
            liminal::graphics::texture *texture;
            glm::vec3 color = {1, 1, 1};
            glm::vec2 position = {};
            float rotation = 0;
            glm::vec2 scale = {1, 1};

            glm::mat4 get_model_matrix() const;
        };
    }
}

#endif
