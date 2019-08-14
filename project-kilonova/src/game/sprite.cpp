#include <game/game.hpp>

namespace pk
{
    sprite::sprite(
        vec3 color,
        pk::texture *color_map,
        vec2 position,
        float rotation,
        vec2 scale)
    {
        glm_vec_copy(color, this->color);
        this->color_map = color_map;
        glm_vec_copy(position, this->position);
        this->rotation = rotation;
        glm_vec_copy(scale, this->scale);
    }

    sprite::~sprite()
    {

    }

    void sprite::calc_model(vec4 *model)
    {
        glm_translate(model, this->position);
        glm_translate(model, vec3{ this->scale[0] * 0.5f, this->scale[1] * 0.5f, 0.0f });
        glm_rotate(model, this->rotation, vec3{ 0.0f, 0.0f, 1.0f });
        glm_translate(model, vec3{ this->scale[0] * -0.5f, this->scale[1] * -0.5f, 0.0f });
        glm_scale(model, this->scale);
    }
}
