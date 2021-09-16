#include "cubemap.hpp"

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

liminal::cubemap::cubemap(std::vector<std::string> filenames)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    {
        for (unsigned int i = 0; i < 6; i++)
        {
            SDL_Surface *surface = IMG_Load(filenames[i].c_str());
            if (!surface)
            {
                std::cerr << "Error: Failed to load cubemap: " << IMG_GetError() << std::endl;
                return;
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                surface->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB,
                surface->w,
                surface->h,
                0,
                surface->format->BytesPerPixel == 4 ? GL_RGBA : surface->format->BytesPerPixel == 1 ? GL_RED
                                                                                                    : GL_RGB,
                GL_UNSIGNED_BYTE,
                surface->pixels);

            SDL_FreeSurface(surface);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

liminal::cubemap::~cubemap()
{
    glDeleteTextures(1, &texture_id);
}
