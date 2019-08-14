#ifndef GAME_WATER_H
#define GAME_WATER_H

struct Water
{
    vec3 position;
    vec2 scale;

    Water(vec3 position, vec2 scale);
    ~Water();
};

#endif
