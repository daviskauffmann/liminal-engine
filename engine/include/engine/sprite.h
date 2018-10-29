#ifndef SPRITE_H
#define SPRITE_H

// TODO: find a way to incorporate this into the renderer
// either that or have a separate sprite_renderer module
// basically, we need somewhere to put the sprite VAO
// this module should be for individual instances

#include <cglm/cglm.h>

struct sprite
{
    vec3 color;
    struct texture *image;
    vec2 position;
    float rotation;
    vec2 scale;
};

int ENGINE_API sprite_init(void);
struct sprite ENGINE_API *sprite_create(
    vec3 color,
    struct texture *image,
    vec2 position,
    float rotation,
    vec2 scale);
void ENGINE_API sprite_calc_model(struct sprite *sprite, mat4 model);
void ENGINE_API sprite_draw(struct sprite *sprite);
void ENGINE_API sprite_destroy(struct sprite *sprite);
void ENGINE_API sprite_quit(void);

#endif
