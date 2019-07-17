#ifndef GAME_CUBEMAP_H
#define GAME_CUBEMAP_H

struct cubemap
{
    GLuint texture_id;
};

struct cubemap *cubemap_create(const char **filenames);
void cubemap_destroy(struct cubemap *cubemap);

#endif
