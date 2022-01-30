#include "grass_block.hpp"

minecraft::tile minecraft::grass_block::get_tile(minecraft::direction direction)
{
    minecraft::tile tile;
    switch (direction)
    {
    case minecraft::direction::DIRECTION_UP:
        tile.x = 2;
        tile.y = 0;
        return tile;
    case minecraft::direction::DIRECTION_DOWN:
        tile.x = 1;
        tile.y = 0;
        return tile;
    default:
        tile.x = 3;
        tile.y = 0;
        return tile;
    }
}
