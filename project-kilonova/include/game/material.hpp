#ifndef GAME_MATERIAL_HPP
#define GAME_MATERIAL_HPP

struct Texture;

struct Material
{
    vec3 color;
    Texture *diffuseMap;
    Texture *specularMap;
    float shininess;
    Texture *normalMap;
    Texture *emissionMap;
    float glow;

    Material(
        vec3 _color,
        Texture *diffuseMap,
        Texture *specularMap,
        float shininess,
        Texture *normalMap,
        Texture *emissionMap,
        float glow);
    ~Material();

    void bind(
        unsigned int diffuseMapIndex,
        unsigned int specularMapIndex,
        unsigned int normalMapIndex,
        unsigned int emissionMapIndex) const;
};

#endif
