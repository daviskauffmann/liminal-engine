#ifndef MINECRAFT_STONE_BLOCK_HPP
#define MINECRAFT_STONE_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class stone_block : public minecraft::block
    {
    public:
        minecraft::tile get_tile(minecraft::direction direction) override;
    };
}

#endif
