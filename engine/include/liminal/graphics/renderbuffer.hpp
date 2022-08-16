#ifndef LIMINAL_GRAPHICS_RENDERBUFFER_HPP
#define LIMINAL_GRAPHICS_RENDERBUFFER_HPP

#include <gl/glew.h>

namespace liminal
{
    class renderbuffer
    {
    public:
        renderbuffer(GLenum internal_format, GLsizei width, GLsizei height);
        ~renderbuffer();

        GLuint get_renderbuffer_id() const;

    private:
        GLuint renderbuffer_id;
    };
}

#endif
