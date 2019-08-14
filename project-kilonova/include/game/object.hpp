#ifndef GAME_OBJECT_HPP
#define GAME_OBJECT_HPP

struct Object
{
    Mesh *mesh;
    Material *material;
    vec3 position;
    vec3 rotation;
    vec3 scale;

    Object(
        Mesh *mesh,
        Material *material,
        vec3 position,
        vec3 rotation,
        vec3 scale);
    ~Object();

    void calcModel(vec4 *model) const;
    void draw(
        unsigned int diffusemapIndex,
        unsigned int specularmapIndex,
        unsigned int normalmapIndex,
        unsigned int emissionmapIndex) const;
};

#endif
