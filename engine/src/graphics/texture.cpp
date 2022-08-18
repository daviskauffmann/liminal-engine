#include <liminal/graphics/texture.hpp>

#include <SDL2/SDL.h>
#include <stb_image.h>
#include <stdexcept>

void liminal::texture::bind(unsigned int index, GLuint texture_id)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

void liminal::texture::unbind(const unsigned int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, 0);
}

liminal::texture::texture(
    const GLenum internal_format,
    const GLsizei width,
    const GLsizei height,
    const GLenum format,
    const GLenum type,
    const liminal::texture_filter filter,
    const liminal::texture_wrap wrap,
    const std::array<GLfloat, 4> &border_color,
    const void *pixels)
{
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            width,
            height,
            0,
            format,
            type,
            pixels);

        switch (filter)
        {
        case liminal::texture_filter::nearest:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case liminal::texture_filter::linear:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        }

        switch (wrap)
        {
        case liminal::texture_wrap::repeat:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        case liminal::texture_wrap::clamp_to_edge:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case liminal::texture_wrap::clamp_to_border:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        }

        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color.data());
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

liminal::texture::texture(const char *const filename, const bool srgb, const bool filter)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, num_components;
    const auto image = stbi_load(filename, &width, &height, &num_components, 0);
    if (!image)
    {
        throw std::runtime_error(stbi_failure_reason());
    }

    GLenum internal_format;
    GLenum format;
    if (num_components == 1)
    {
        internal_format = format = GL_RED;
    }
    else if (num_components == 3)
    {
        internal_format = srgb ? GL_SRGB : GL_RGB;
        format = GL_RGB;
    }
    else /*if (num_components == 4)*/
    {
        internal_format = srgb ? GL_SRGB_ALPHA : GL_RGBA;
        format = GL_RGBA;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    {
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internal_format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            image);

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter ? GL_LINEAR : GL_NEAREST);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);
}

liminal::texture::~texture()
{
    glDeleteTextures(1, &texture_id);
}

GLuint liminal::texture::get_texture_id() const
{
    return texture_id;
}

void liminal::texture::bind(const unsigned int index) const
{
    bind(index, texture_id);
}
