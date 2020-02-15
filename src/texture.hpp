#ifndef TEXTURE_HPP
#ifndef TEXTURE_HPP

#include <string>

#include <GL/glew.h>

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
