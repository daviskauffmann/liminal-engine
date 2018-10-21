#include <engine/engine.h>

struct texture *texture_create(const char *file)
{
    struct texture *texture = malloc(sizeof(struct texture));

    if (!texture)
    {
        error("Couldn't allocate texture");

        return NULL;
    }

    glGenTextures(1, &texture->texture);

    glBindTexture(GL_TEXTURE_2D, texture->texture);

    SDL_Surface *surface = IMG_Load(file);

    if (!surface)
    {
        error(IMG_GetError());

        return NULL;
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        surface->w,
        surface->h,
        0,
        surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        surface->pixels);

    SDL_FreeSurface(surface);

    // mip mapping
    glGenerateMipmap(GL_TEXTURE_2D);

    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // lod
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void texture_destroy(struct texture *texture)
{
    glDeleteTextures(1, &texture->texture);

    free(texture);
}
