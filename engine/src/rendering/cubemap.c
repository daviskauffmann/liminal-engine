#include <engine/engine.h>

struct cubemap *cubemap_create(const char **files)
{
    struct cubemap *cubemap = malloc(sizeof(struct cubemap));

    if (!cubemap)
    {
        error("Couldn't create cubemap");

        return NULL;
    }

    glGenTextures(1, &cubemap->texture_id);

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->texture_id);

    for (unsigned int i = 0; i < 6; i++)
    {
        // load image
        SDL_Surface *surface = IMG_Load(files[i]);

        if (!surface)
        {
            error(IMG_GetError());

            return NULL;
        }

        // create texture
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGB,
            surface->w,
            surface->h,
            0,
            surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
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
