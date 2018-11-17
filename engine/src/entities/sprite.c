#include <engine/engine.h>

struct sprite *sprite_create(
    vec3 color,
    struct texture *image,
    vec2 position,
    float rotation,
    vec2 scale)
{
    struct sprite *sprite = malloc(sizeof(struct sprite));

    if (!sprite)
    {
        printf("Error: Couldn't allocate sprite\n");

        return NULL;
    }

    glm_vec_copy(color, sprite->color);
    sprite->image = image;
    glm_vec_copy(position, sprite->position);
    sprite->rotation = rotation;
    glm_vec_copy(scale, sprite->scale);

    return sprite;
}

void sprite_calc_model(struct sprite *sprite, mat4 model)
{
    glm_translate(model, sprite->position);
    vec3 center = { sprite->scale[0] * 0.5f, sprite->scale[1] * 0.5f, 0.0f };
    glm_translate(model, center);
    glm_rotate(model, sprite->rotation, GLM_ZUP);
    vec3 revert = { sprite->scale[0] * -0.5f, sprite->scale[1] * -0.5f, 0.0f };
    glm_translate(model, revert);
    glm_scale(model, sprite->scale);
}

void sprite_destroy(struct sprite *sprite)
{
    free(sprite);
}
