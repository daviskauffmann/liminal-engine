#ifndef MINECRAFT_GRASS_BLOCK_HPP
#define MINECRAFT_GRASS_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class grass_block : public minecraft::block
    {
    public:
        glm::ivec2 get_tile(const minecraft::direction direction) const override;
    };
}

#endif
