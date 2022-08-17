#include <liminal/graphics/cubemap.hpp>

#include <SDL2/SDL_image.h>
#include <stdexcept>

void liminal::cubemap::bind(unsigned int index, GLuint texture_id)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
}

void liminal::cubemap::unbind(unsigned int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

liminal::cubemap::cubemap(
    const GLenum internal_format,
    const GLsizei width,
    const GLsizei height,
    const GLenum format,
    const GLenum type,
    const liminal::texture_filter filter,
    const liminal::texture_wrap wrap)
{
    glGenTextures(1, &cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
    {
        for (GLenum face = 0; face < 6; face++)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                0,
                internal_format,
                width,
                height,
                0,
                format,
                type,
                nullptr);
        }

        switch (filter)
        {
        case liminal::texture_filter::nearest:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case liminal::texture_filter::linear:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        }

        switch (wrap)
        {
        case liminal::texture_wrap::repeat:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
            break;
        case liminal::texture_wrap::clamp_to_edge:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            break;
        case liminal::texture_wrap::clamp_to_border:
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            break;
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

liminal::cubemap::cubemap(const std::array<std::string, num_sides> &filenames)
{
    glGenTextures(1, &cubemap_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_id);
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
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

liminal::cubemap::~cubemap()
{
    glDeleteTextures(1, &cubemap_id);
}

GLuint liminal::cubemap::get_cubemap_id() const
{
    return cubemap_id;
}

void liminal::cubemap::bind(const unsigned int index) const
{
    bind(index, cubemap_id);
}
