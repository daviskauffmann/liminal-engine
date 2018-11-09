#include <engine/engine.h>

struct water *water_create(vec3 position, vec2 scale)
{
    struct water *water = malloc(sizeof(struct water));

    if (!water)
    {
        error("Couldn't allocate water");

        return NULL;
    }

    glm_vec_copy(position, water->position);
    glm_vec_copy(scale, water->scale);

    return water;
}

void water_destroy(struct water *water)
{
    free(water);
}
