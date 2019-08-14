#ifndef GAME_CUBEMAP_H
#define GAME_CUBEMAP_H

struct Cubemap
{
    GLuint textureId;

    Cubemap(std::vector<std::string> filenames);
    ~Cubemap();
};

#endif
