#ifndef LIMINAL_GRAPHICS_TEXTURE_HPP
#define LIMINAL_GRAPHICS_TEXTURE_HPP

#include <array>
#include <gl/glew.h>

namespace liminal
{
    enum class texture_filter
    {
        nearest,
        linear
    };

    enum class texture_wrap
    {
        repeat,
        clamp_to_edge,
        clamp_to_border
    };

    class texture
    {
    public:
        static void unbind(unsigned int index);

        texture(
            GLenum internal_format,
            GLsizei width,
            GLsizei height,
            GLenum format,
            GLenum type,
            liminal::texture_filter filter = liminal::texture_filter::nearest,
            liminal::texture_wrap wrap = liminal::texture_wrap::repeat,
            const std::array<GLfloat, 4> &border_color = {0, 0, 0, 0});
        texture(const char *filename, bool srgb = false, bool filter = true);
        ~texture();

        GLuint get_texture_id() const;

        void bind(unsigned int index) const;

    private:
        GLuint texture_id;
    };
}

#endif
