#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <string>

namespace pk
{
struct texture
{
    GLuint texture_id;

    texture(const std::string &filename, bool srgb = false);
    ~texture();

    void bind(unsigned int index) const;
};
} // namespace pk

#endif
