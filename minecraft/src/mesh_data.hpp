#ifndef MINECRAFT_MESH_DATA_HPP
#define MINECRAFT_MESH_DATA_HPP

#include <liminal/liminal.hpp>
#include <vector>

namespace minecraft
{
    struct mesh_data
    {
        std::vector<liminal::vertex> vertices;
        std::vector<unsigned int> indices;

        void add_quad_indices();
    };
}

#endif
