#ifndef GAME_HDR_TEXTURE_HPP
#define GAME_HDR_TEXTURE_HPP

#include <string>

#include <GL/glew.h>

namespace pk
{
struct hdr_texture
{
    GLuint texture_id;

    hdr_texture(const std::string &filename);
    ~hdr_texture();

    void bind(unsigned int index) const;
};
} // namespace pk

#endif
