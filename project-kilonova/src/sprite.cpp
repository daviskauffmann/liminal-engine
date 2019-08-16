#include "sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace pk
{
    sprite::sprite(
        glm::vec3 color,
        pk::texture *color_map,
        glm::vec2 position,
        float rotation,
        glm::vec2 scale)
    {
        this->color = color;
        this->color_map = color_map;
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
    }

    sprite::~sprite()
    {

    }

    glm::mat4 sprite::calc_model() const
    {
        glm::mat4 model(1.0f);

        model = glm::translate(model, glm::vec3(this->position.x, this->position.y, 0.0f));
        //model = glm::rotate(model, );
        model = glm::scale(model, glm::vec3(this->scale.x, this->scale.y, 1.0f));

        return model;

        //glm_translate(model, this->position);
        //glm_translate(model, vec3{ this->scale[0] * 0.5f, this->scale[1] * 0.5f, 0.0f });
        //glm_rotate(model, this->rotation, vec3{ 0.0f, 0.0f, 1.0f });
        //glm_translate(model, vec3{ this->scale[0] * -0.5f, this->scale[1] * -0.5f, 0.0f });
        //glm_scale(model, this->scale);
    }
}
