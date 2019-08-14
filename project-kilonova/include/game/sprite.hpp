#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

struct Sprite
{
    vec3 color;
    Texture *colorMap;
    vec2 position;
    float rotation;
    vec2 scale;

    Sprite(
        vec3 color,
        Texture *colorMap,
        vec2 position,
        float rotation,
        vec2 scale);
    ~Sprite();

    void calcModel(mat4 *model) const;
};

#endif
