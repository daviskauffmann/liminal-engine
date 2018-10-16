#ifndef BITMAP_H
#define BITMAP_H

struct bitmap
{
    int width;
    int height;
    unsigned int *pixels;
};

struct bitmap *bitmap_create(const char *file);
unsigned int bitmap_get_pixel(struct bitmap *bitmap, int x, int y);
void bitmap_set_pixel(struct bitmap *bitmap, int x, int y, unsigned int color);
void bitmap_destroy(struct bitmap *bitmap);

#endif
