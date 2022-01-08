#include <liminal/graphics/texture.hpp>

#include <iostream>
#include <SDL2/SDL.h>
#include <stb_image.h>

liminal::texture::texture(const std::string &filename, bool srgb, bool filter)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, num_components;
    unsigned char *image = stbi_load(filename.c_str(), &width, &height, &num_components, 0);
    if (!image)
    {
        std::cerr << "Error: Failed to load texture: " << stbi_failure_reason() << std::endl;
        return;
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
    else if (num_components == 4)
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

void liminal::texture::bind(unsigned int index) const
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}
