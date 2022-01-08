#ifndef MINECRAFT_CHUNK_HPP
#define MINECRAFT_CHUNK_HPP

#include <liminal.hpp>

class block;

class chunk
{
public:
    block *blocks[16][16][16];

    block *get_block(int x, int y, int z);
    liminal::mesh *render(liminal::texture *tiles_texture);
};

#endif
