#include "air_block.hpp"

void minecraft::air_block::add_to_mesh(const minecraft::chunk *const, const int, const int, const int, minecraft::mesh_data *const) const
{
}

bool minecraft::air_block::is_solid(const minecraft::direction) const
{
    return false;
}
