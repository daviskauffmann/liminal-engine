#include <liminal/graphics/cubemap.hpp>

#include <SDL2/SDL_image.h>
#include <stdexcept>

liminal::cubemap::cubemap(const std::array<std::string, num_sides> &filenames)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    {
        for (GLenum side_index = 0; side_index < num_sides; side_index++)
        {
            const auto surface = IMG_Load(filenames.at(side_index).c_str());
            if (!surface)
            {
                throw std::runtime_error(IMG_GetError());
            }

            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + side_index,
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
