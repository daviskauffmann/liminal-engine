#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

struct texture
{
    GLuint texture_id;
};

struct texture ENGINE_API *texture_create(int width, int height, unsigned char bytes_per_pixel, const void *pixels);
void ENGINE_API texture_bind(struct texture *texture, unsigned int index);
void ENGINE_API texture_destroy(struct texture *texture);

#endif
