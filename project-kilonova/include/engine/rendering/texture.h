#ifndef ENGINE_TEXTURE_H
#define ENGINE_TEXTURE_H

struct texture
{
    GLuint texture_id;
};

struct texture *texture_create(const char *filename);
void texture_bind(struct texture *texture, unsigned int index);
void texture_destroy(struct texture *texture);

#endif
