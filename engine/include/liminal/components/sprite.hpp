#ifndef LIMINAL_COMPONENTS_SPRITE_HPP
#define LIMINAL_COMPONENTS_SPRITE_HPP

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <liminal/graphics/texture.hpp>

namespace liminal
{
    class sprite
    {
    public:
        liminal::texture *texture = nullptr;
        glm::vec3 color = glm::vec3(1);
        glm::vec2 position = glm::vec2();
        float rotation = 0;
        glm::vec2 scale = glm::vec2(1);

        glm::mat4 get_model_matrix() const;
    };
}

#endif
