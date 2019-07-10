#include <engine/engine.h>

struct cubemap *cubemap_create(int *width_array, int *height_array, unsigned char *bytes_per_pixel_array, const void **pixels_array)
{
    struct cubemap *cubemap = malloc(sizeof(struct cubemap));

    if (!cubemap)
    {
        printf("Error: Couldn't create cubemap\n");

        return NULL;
    }

    glGenTextures(1, &cubemap->texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture_id);

    for (unsigned int i = 0; i < 6; i++)
    {
        int width = width_array[i];
        int height = height_array[i];
        unsigned char bytes_per_pixel = bytes_per_pixel_array[i];
        const void *pixels = pixels_array[i];

        // create texture
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            bytes_per_pixel == 4 ? GL_RGBA : GL_RGB,
            width,
            height,
            0,
            bytes_per_pixel == 4 ? GL_RGBA : bytes_per_pixel == 1 ? GL_RED : GL_RGB,
            GL_UNSIGNED_BYTE,
            pixels);
    }

    // wrapping
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // filtering
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return cubemap;
}

void cubemap_destroy(struct cubemap *cubemap)
{
    glDeleteTextures(1, &cubemap->texture_id);
    free(cubemap);
}
