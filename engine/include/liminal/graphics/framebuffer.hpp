#ifndef LIMINAL_GRAPHICS_FRAMEBUFFER_HPP
#define LIMINAL_GRAPHICS_FRAMEBUFFER_HPP

#include "texture.hpp"
#include <array>
#include <gl/glew.h>
#include <vector>

namespace liminal
{
    class framebuffer
    {
    public:
        static void unbind();

        framebuffer(GLsizei width, GLsizei height);
        ~framebuffer();

        GLsizei get_width() const;
        GLsizei get_height() const;
        GLuint get_framebuffer_id() const;

        void set_draw_buffer(GLenum mode) const;
        void set_read_buffer(GLenum mode) const;

        void attach_color_texture(GLuint texture_id) const;
        void attach_depth_renderbuffer(GLuint renderbuffer_id) const;
        void attach_depth_texture(GLuint texture_id) const;

        void validate() const;

        void bind() const;

    private:
        GLsizei width;
        GLsizei height;
        GLuint framebuffer_id;

        mutable std::vector<GLuint> color_texture_ids;
    };
}

#endif
