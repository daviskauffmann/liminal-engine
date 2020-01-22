#ifndef GAME_CUBEMAP_HPP
#define GAME_CUBEMAP_HPP

#include <string>
#include <vector>

#include <GL/glew.h>

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
