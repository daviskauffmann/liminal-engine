#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct texture
{
    GLuint texture;
};

struct texture ENGINE_API *texture_create(const char *file);
void ENGINE_API texture_destroy(struct texture *texture);

#endif
