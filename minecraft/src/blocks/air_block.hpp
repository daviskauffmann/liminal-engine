#ifndef MINECRAFT_AIR_BLOCK_HPP
#define MINECRAFT_AIR_BLOCK_HPP

#include "../block.hpp"

namespace minecraft
{
    class air_block : public minecraft::block
    {
        bool is_solid(const minecraft::direction direction) const override;
        void add_to_mesh(const minecraft::chunk *const chunk, const int x, const int y, const int z, minecraft::mesh_data *const mesh_data) const override;
    };
}

#endif
