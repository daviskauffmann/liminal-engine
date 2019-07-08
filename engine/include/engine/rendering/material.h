#ifndef MATERIAL_H
#define MATERIAL_H

struct material
{
    vec3 color;
    struct texture *diffuse_map;
    struct texture *specular_map;
    float shininess; // TODO: rename this to shine_damper?
    struct texture *normal_map;
    struct texture *emission_map;
    float glow;
};

struct material ENGINE_API *material_create(
    vec3 color,
    struct texture *diffuse_map,
    struct texture *specular_map,
    float shininess,
    struct texture *normal_map,
    struct texture *emission_map,
    float glow);
void ENGINE_API material_bind(
    struct material *material,
    unsigned int diffuse_map_index,
    unsigned int specular_map_index,
    unsigned int normal_map_index,
    unsigned int emission_map_index);
void ENGINE_API material_destroy(struct material *material);

#endif
