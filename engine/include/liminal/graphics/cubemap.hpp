#ifndef LIMINAL_GRAPHICS_CUBEMAP_HPP
#define LIMINAL_GRAPHICS_CUBEMAP_HPP

#include "texture.hpp"
#include <array>
#include <gl/glew.h>
#include <string>

namespace liminal
{
    class cubemap
    {
    public:
        static constexpr std::size_t num_sides = 6;

        static void bind(unsigned int index, GLuint texture_id);
        static void unbind(unsigned int index);

        cubemap(
            GLenum internal_format,
            GLsizei width,
            GLsizei height,
            GLenum format,
            GLenum type,
            liminal::texture_filter filter = liminal::texture_filter::nearest,
            liminal::texture_wrap wrap = liminal::texture_wrap::repeat);
        cubemap(const std::array<std::string, num_sides> &filenames);
        ~cubemap();

        GLuint get_cubemap_id() const;

        void bind(unsigned int index) const;

    private:
        GLuint cubemap_id;
    };
}

#endif
