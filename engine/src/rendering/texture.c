#include <engine/engine.h>

// TODO: a way to load textures internally within the engine
// right now, we expect the user to load their own textures however they want and pass the pixel data into the engine
// however, it would be nice to be able to load stuff like the water dudv map within the engine itself and not force the user to supply one

struct texture *texture_create(int width, int height, unsigned char bytes_per_pixel, const void *pixels)
{
    struct texture *texture = malloc(sizeof(struct texture));

    if (!texture)
    {
        printf("Error: Couldn't allocate texture\n");

        return NULL;
    }

    glGenTextures(1, &texture->texture_id);

    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        bytes_per_pixel == 4 ? GL_RGBA : GL_RGB,
        width,
        height,
        0,
        bytes_per_pixel == 4 ? GL_RGBA : bytes_per_pixel == 1 ? GL_RED : GL_RGB,
        GL_UNSIGNED_BYTE,
        pixels);

    // mip mapping
    glGenerateMipmap(GL_TEXTURE_2D);

    // wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // lod
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void texture_bind(struct texture *texture, unsigned int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
}

void texture_destroy(struct texture *texture)
{
    glDeleteTextures(1, &texture->texture_id);

    free(texture);
}
