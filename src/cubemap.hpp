#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <GL/glew.h>
#include <string>
#include <vector>

namespace pk
{
    struct cubemap
    {
        GLuint texture_id;

        cubemap(std::vector<std::string> filenames);
        ~cubemap();
    };
} // namespace pk

#endif
