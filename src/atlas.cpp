#include "atlas.hpp"

#include <iostream>

namespace pk
{
    atlas::atlas(const std::string &filename, GLuint size, GLuint texture_size)
        : texture(filename),
          size(size),
          texture_size(texture_size)
    {
    }

    std::vector<GLfloat> atlas::get_texture_coords(const glm::vec2 &location)
    {
        static auto textures_per_row = size / texture_size;
        static auto unit_size = 1.0f / (float)textures_per_row;

        auto min_x = location.x * unit_size;
        auto min_y = location.y * unit_size;
        auto max_x = min_x + unit_size;
        auto max_y = min_y + unit_size;

        return {
            max_x, max_y,
            min_x, max_y,
            min_x, min_y,
            max_x, min_y};
    }

} // namespace pk
