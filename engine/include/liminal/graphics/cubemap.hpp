#ifndef LIMINAL_GRAPHICS_CUBEMAP_HPP
#define LIMINAL_GRAPHICS_CUBEMAP_HPP

#include <array>
#include <gl/glew.h>
#include <string>

namespace liminal
{
    // TODO: remove this class? it is unused right now
    class cubemap
    {
    public:
        static constexpr std::size_t num_sides = 6;

        GLuint texture_id;

        cubemap(const std::array<std::string, num_sides> &filenames);
        ~cubemap();
    };
}

#endif
