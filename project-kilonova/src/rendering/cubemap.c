#include <engine/engine.h>

struct cubemap *cubemap_create(const char **filenames)
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
        SDL_Surface *surface = IMG_Load(filenames[i]);

        if (!surface)
        {
            printf("Error: Couldn't load image\n");

            return NULL;
        }

        // create texture
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
            surface->w,
            surface->h,
            0,
            surface->format->BytesPerPixel == 4 ? GL_RGBA : surface->format->BytesPerPixel == 1 ? GL_RED : GL_RGB,
            GL_UNSIGNED_BYTE,
            surface->pixels);

        SDL_FreeSurface(surface);
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
