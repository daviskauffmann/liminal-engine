#ifndef GAME_MATERIAL_H
#define GAME_MATERIAL_H

struct material
{
    vec3 color;
    struct texture *diffuse_map;
    struct texture *specular_map;
    float shininess;
    struct texture *normal_map;
    struct texture *emission_map;
    float glow;
};

struct material *material_create(
    vec3 color,
    struct texture *diffuse_map,
    struct texture *specular_map,
    float shininess,
    struct texture *normal_map,
    struct texture *emission_map,
    float glow);
void material_bind(
    struct material *material,
    unsigned int diffuse_map_index,
    unsigned int specular_map_index,
    unsigned int normal_map_index,
    unsigned int emission_map_index);
void material_destroy(struct material *material);

#endif
