#ifndef GAME_TEXTURE_HPP
#define GAME_TEXTURE_HPP

#include <string>

#include <GL/glew.h>

namespace pk
{
struct texture
{
    GLuint texture_id;

    texture(const std::string &filename);
    ~texture();

    void bind(unsigned int index) const;
};
} // namespace pk

#endif
