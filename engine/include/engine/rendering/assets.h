#ifndef ASSETS_H
#define ASSETS_H

struct assets
{
    struct texture *default_texture;

    struct material *default_material;

    struct mesh *quad_mesh;
    struct mesh *cube_mesh;
};

extern struct assets ENGINE_API assets;

int assets_load(void);
void assets_unload(void);

#endif
