#ifndef LIMINAL_GRAPHICS_CUBEMAP_HPP
#define LIMINAL_GRAPHICS_CUBEMAP_HPP

#include "texture.hpp"
#include <array>
#include <gl/glew.h>
#include <string>

namespace liminal
{
    namespace graphics
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
                liminal::graphics::texture_filter filter = liminal::graphics::texture_filter::nearest,
                liminal::graphics::texture_wrap wrap = liminal::graphics::texture_wrap::repeat);
            cubemap(const std::array<std::string, num_sides> &filenames);
            ~cubemap();
            cubemap(const cubemap &other) = delete;
            cubemap &operator=(const cubemap &other) = delete;
            cubemap(cubemap &&other) = delete;
            cubemap &operator=(cubemap &&other) = delete;

            GLuint get_cubemap_id() const;

            void bind(unsigned int index) const;

        private:
            GLuint cubemap_id;
        };
    }
}

#endif
