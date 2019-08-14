#ifndef GAME_WATER_HPP
#define GAME_WATER_HPP

namespace pk
{
    struct water
    {
        vec3 position;
        vec2 scale;

        water(vec3 position, vec2 scale);
        ~water();
    };
}

#endif
