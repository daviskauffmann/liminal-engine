#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

struct object
{
    struct mesh *mesh;
    struct material *material;
    vec3 position;
    vec3 rotation;
    vec3 scale;
};

struct object *object_create(
    struct mesh *mesh,
    struct material *material,
    vec3 position,
    vec3 rotation,
    vec3 scale);
void object_calc_model(struct object *object, vec4 *model);
void object_draw(
    struct object *object,
    unsigned int diffuse_map_index,
    unsigned int specular_map_index,
    unsigned int normal_map_index,
    unsigned int emission_map_index);
void object_destroy(struct object *object);

#endif
