#ifndef MINECRAFT_AIR_BLOCK_HPP
#define MINECRAFT_AIR_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class air_block : public minecraft::block
    {
        bool is_solid(minecraft::direction direction) const override;
        void add_to_mesh(const minecraft::chunk *chunk, int x, int y, int z, minecraft::mesh_data *mesh_data) const override;
    };
}

#endif
