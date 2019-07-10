#ifndef ENGINE_CUBEMAP_H
#define ENGINE_CUBEMAP_H

struct cubemap
{
    GLuint texture_id;
};

struct cubemap ENGINE_API *cubemap_create(int *width_array, int *height_array, unsigned char *bytes_per_pixel_array, const void **pixels_array);
void ENGINE_API cubemap_destroy(struct cubemap *cubemap);

#endif
