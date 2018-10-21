#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct texture
{
    GLuint texture;
};

struct texture *texture_create(const char *file);
void texture_destroy(struct texture *texture);

#endif
