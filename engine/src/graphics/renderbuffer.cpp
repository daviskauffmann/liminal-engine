#include <liminal/graphics/renderbuffer.hpp>

liminal::renderbuffer::renderbuffer(
    const GLenum internal_format,
    const GLsizei width,
    const GLsizei height)
{
    glGenRenderbuffers(1, &renderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    {
        glRenderbufferStorage(
            GL_RENDERBUFFER,
            internal_format,
            width,
            height);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

liminal::renderbuffer::~renderbuffer()
{
    glDeleteRenderbuffers(1, &renderbuffer_id);
}

GLuint liminal::renderbuffer::get_renderbuffer_id() const
{
    return renderbuffer_id;
}
