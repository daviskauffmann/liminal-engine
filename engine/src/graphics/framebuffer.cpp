#include <liminal/graphics/framebuffer.hpp>

#include <stdexcept>

void liminal::framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

liminal::framebuffer::framebuffer(const GLsizei width, const GLsizei height)
    : width(width),
      height(height)
{
    glGenFramebuffers(1, &framebuffer_id);
}

liminal::framebuffer::~framebuffer()
{
    glDeleteFramebuffers(1, &framebuffer_id);
}

GLsizei liminal::framebuffer::get_width() const
{
    return width;
}

GLsizei liminal::framebuffer::get_height() const
{
    return height;
}

GLuint liminal::framebuffer::get_framebuffer_id() const
{
    return framebuffer_id;
}

void liminal::framebuffer::set_draw_buffer(const GLenum mode) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    glDrawBuffer(mode);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::framebuffer::set_read_buffer(const GLenum mode) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    glReadBuffer(mode);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::framebuffer::add_color_texture(const GLuint texture_id) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    {
        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(color_texture_ids.size()),
            GL_TEXTURE_2D,
            texture_id,
            0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    color_texture_ids.push_back(texture_id);
}

void liminal::framebuffer::add_depth_renderbuffer(const GLuint renderbuffer_id) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    {
        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            renderbuffer_id);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::framebuffer::add_depth_texture(const GLuint texture_id) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    {
        glFramebufferTexture(
            GL_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT,
            texture_id,
            0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::framebuffer::validate() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    {
        std::vector<GLenum> color_attachments;
        for (GLenum color_attachment = GL_COLOR_ATTACHMENT0; color_attachment < GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(color_texture_ids.size()); color_attachment++)
        {
            color_attachments.push_back(color_attachment);
        }
        glDrawBuffers(static_cast<GLsizei>(color_attachments.size()), color_attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            const auto error_code = glGetError();
            const auto error_string = gluErrorString(error_code);
            throw std::runtime_error(reinterpret_cast<const char *>(error_string));
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void liminal::framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id);
    glViewport(0, 0, width, height);
}
