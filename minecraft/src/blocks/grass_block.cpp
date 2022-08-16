#include "grass_block.hpp"

glm::ivec2 minecraft::grass_block::get_tile(const minecraft::direction direction) const
{
    switch (direction)
    {
    case minecraft::direction::up:
        return glm::ivec2(2, 0);
    case minecraft::direction::down:
        return glm::ivec2(1, 0);
    default:
        return glm::ivec2(3, 0);
    }
}
