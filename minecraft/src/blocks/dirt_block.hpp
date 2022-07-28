#ifndef MINECRAFT_DIRT_BLOCK_HPP
#define MINECRAFT_DIRT_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class dirt_block : public minecraft::block
    {
    public:
        glm::ivec2 get_tile(minecraft::direction direction) const override;
    };
}

#endif
