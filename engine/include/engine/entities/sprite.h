#ifndef SPRITE_H
#define SPRITE_H

struct sprite
{
    vec3 color;
    struct texture *color_map;
    vec2 position;
    float rotation;
    vec2 scale;
};

struct sprite ENGINE_API *sprite_create(
    vec3 color,
    struct texture *color_map,
    vec2 position,
    float rotation,
    vec2 scale);
void ENGINE_API sprite_calc_model(struct sprite *sprite, mat4 model);
void ENGINE_API sprite_destroy(struct sprite *sprite);

#endif
