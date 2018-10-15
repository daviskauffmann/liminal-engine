#include <GL/glew.h>
#include <malloc.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "texture.h"

struct texture *texture_create(const char *file)
{
    struct texture *texture = calloc(1, sizeof(struct texture));

    glGenTextures(1, &texture->texture);

    glBindTexture(GL_TEXTURE_2D, texture->texture);

    SDL_Surface *surface = IMG_Load(file);

    if (!surface)
    {
        SDL_SetError(IMG_GetError());

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

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void texture_bind(struct texture *texture, unsigned char num)
{
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, texture->texture);
}

void texture_unbind(unsigned char num)
{
    glActiveTexture(GL_TEXTURE0 + num);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_destroy(struct texture *texture)
{
    glDeleteTextures(1, &texture->texture);

    free(texture);
}
