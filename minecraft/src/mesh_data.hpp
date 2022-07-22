#ifndef MINECRAFT_MESH_DATA_HPP
#define MINECRAFT_MESH_DATA_HPP

#include <liminal/liminal.hpp>
#include <vector>

namespace minecraft
{
    class mesh_data
    {
    public:
        std::vector<liminal::vertex> vertices;
        std::vector<unsigned int> indices;

        void add_quad_indices();
    };
}

#endif
