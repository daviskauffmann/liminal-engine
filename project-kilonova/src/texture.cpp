#include "texture.hpp"

#include <iostream>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

namespace pk
{
texture::texture(const std::string &filename)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    SDL_Surface *surface = IMG_Load(filename.c_str());
    if (!surface)
    {
        std::cout << "Error: Couldn't load image " << filename << std::endl;
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
