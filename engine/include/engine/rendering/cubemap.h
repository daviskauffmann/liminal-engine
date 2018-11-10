#ifndef CUBEMAP_H
#define CUBEMAP_H

struct cubemap
{
    GLuint texture_id;
};

struct cubemap ENGINE_API *cubemap_create(const char **files);
void ENGINE_API cubemap_destroy(struct cubemap *cubemap);

#endif
