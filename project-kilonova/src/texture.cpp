#include "texture.hpp"

#include <iostream>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

namespace pk
{
texture::texture(const std::string &filename, bool srgb)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cout << "Error: Couldn't load image " << filename << std::endl;
    }
    GLenum internal_format;
    GLenum format;
    if (surface->format->BytesPerPixel == 1)
    {
        internal_format = format = GL_RED;
    }
    else if (surface->format->BytesPerPixel == 3)
    {
        internal_format = srgb ? GL_SRGB : GL_RGB;
        format = GL_RGB;
    }
    else if (surface->format->BytesPerPixel == 4)
    {
        internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;
        format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal_format,
        surface->w,
        surface->h,
        0,
        format,
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
}

texture::~texture()
{
    glDeleteTextures(1, &texture_id);
}

void texture::bind(unsigned int index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}
} // namespace pk
