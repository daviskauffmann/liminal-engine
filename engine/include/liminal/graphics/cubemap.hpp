#ifndef LIMINAL_GRAPHICS_CUBEMAP_HPP
#define LIMINAL_GRAPHICS_CUBEMAP_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

namespace liminal
{
    // TODO: remove this class? it is unused right now
    class cubemap
    {
    public:
        GLuint texture_id;

        cubemap(const std::vector<std::string> &filenames);
        ~cubemap();
    };
}

#endif
