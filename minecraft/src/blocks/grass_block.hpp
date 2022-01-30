#ifndef MINECRAFT_GRASS_BLOCK_HPP
#define MINECRAFT_GRASS_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class grass_block : public minecraft::block
    {
    public:
        minecraft::tile get_tile(minecraft::direction direction) override;
    };
}

#endif
