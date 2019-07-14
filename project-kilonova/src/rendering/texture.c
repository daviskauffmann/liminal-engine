#include <engine/engine.h>

struct texture *texture_create(const char *filename)
{
    struct texture *texture = malloc(sizeof(struct texture));

    if (!texture)
    {
        printf("Error: Couldn't allocate texture\n");

        return NULL;
    }

    glGenTextures(1, &texture->texture_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);

    SDL_Surface *surface = IMG_Load(filename);

    if (!surface)
    {
        printf("Error: Couldn't load image\n");

        return NULL;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
        surface->w,
        surface->h,
        0,
        surface->format->BytesPerPixel == 4 ? GL_RGBA : surface->format->BytesPerPixel == 1 ? GL_RED : GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels);

    SDL_FreeSurface(surface);

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
