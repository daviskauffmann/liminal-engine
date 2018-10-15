#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct texture
{
    GLuint texture;
};

struct texture *texture_create(const char *file);
void texture_bind(struct texture *texture, unsigned char num);
void texture_unbind(unsigned char num);
void texture_destroy(struct texture *texture);

#endif
