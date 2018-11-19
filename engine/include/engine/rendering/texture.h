#ifndef TEXTURE_H
#define TEXTURE_H

struct texture
{
    GLuint texture_id;
};

struct texture ENGINE_API *texture_create(int width, int height, unsigned char bytes_per_pixel, const void *pixels);
void ENGINE_API texture_destroy(struct texture *texture);

#endif
