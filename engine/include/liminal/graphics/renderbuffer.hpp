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
        renderbuffer(const renderbuffer &other) = delete;
        renderbuffer &operator=(const renderbuffer &other) = delete;
        renderbuffer(renderbuffer &&other) = delete;
        renderbuffer &operator=(renderbuffer &&other) = delete;

        GLuint get_renderbuffer_id() const;

    private:
        GLuint renderbuffer_id;
    };
}

#endif
