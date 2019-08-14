#ifndef GAME_CUBEMAP_HPP
#define GAME_CUBEMAP_HPP

namespace pk
{
    struct cubemap
    {
        GLuint texture_id;

        cubemap(std::vector<std::string> filenames);
        ~cubemap();
    };
}

#endif
