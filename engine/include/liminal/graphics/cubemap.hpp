#ifndef LIMINAL_GRAPHICS_CUBEMAP_HPP
#define LIMINAL_GRAPHICS_CUBEMAP_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

namespace liminal
{
    class cubemap
    {
    public:
        GLuint texture_id;

        cubemap(std::vector<std::string> filenames);
        ~cubemap();
    };
}

#endif
