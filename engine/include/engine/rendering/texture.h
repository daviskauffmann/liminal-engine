#ifndef TEXTURE_H
#define TEXTURE_H

struct texture
{
    GLuint texture_id;
};

struct texture ENGINE_API *texture_create(const char *file);
void ENGINE_API texture_destroy(struct texture *texture);

#endif
