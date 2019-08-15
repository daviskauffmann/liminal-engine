#ifndef GAME_SPRITE_HPP
#define GAME_SPRITE_HPP

#include <cglm/cglm.h>

#include "texture.hpp"

namespace pk
{
    struct sprite
    {
        vec3 color;
        texture *color_map;
        vec2 position;
        float rotation;
        vec2 scale;

        sprite(
            vec3 color,
            texture *colorMap,
            vec2 position,
            float rotation,
            vec2 scale);
        ~sprite();

        void calc_model(vec4 *model);
    };
}

#endif
