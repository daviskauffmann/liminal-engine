#ifndef BITMAP_H
#define BITMAP_H

struct bitmap
{
    int width;
    int height;
    unsigned int *pixels;
};

struct bitmap ENGINE_API *bitmap_create(const char *file);
unsigned int ENGINE_API bitmap_get_pixel(struct bitmap *bitmap, int x, int y);
void ENGINE_API bitmap_set_pixel(struct bitmap *bitmap, int x, int y, unsigned int color);
void ENGINE_API bitmap_destroy(struct bitmap *bitmap);

#endif
