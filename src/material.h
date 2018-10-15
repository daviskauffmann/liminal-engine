#ifndef MATERIAL_H
#define MATERIAL_H

struct texture;

struct material
{
    struct texture *diffuse;
    struct texture *specular;
    struct texture *emission;
    float shininess;
    float glow;
};

struct material *material_create(
    struct texture *diffuse,
    struct texture *specular,
    struct texture *emission,
    float shininess,
    float glow);
void material_bind(struct material *material);
void material_unbind(void);
void material_destroy(struct material *material);

#endif
