#ifndef ENGINE_WATER_H
#define ENGINE_WATER_H

struct water
{
    vec3 position;
    vec2 scale;
};

struct water ENGINE_API *water_create(vec3 position, vec2 scale);
void ENGINE_API water_destroy(struct water *water);

#endif
