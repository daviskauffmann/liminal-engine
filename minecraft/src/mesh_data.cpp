#include "mesh_data.hpp"

void minecraft::mesh_data::add_quad_indices()
{
    const auto size = vertices.size();

    indices.push_back((unsigned int)(size - 4));
    indices.push_back((unsigned int)(size - 3));
    indices.push_back((unsigned int)(size - 2));

    indices.push_back((unsigned int)(size - 4));
    indices.push_back((unsigned int)(size - 2));
    indices.push_back((unsigned int)(size - 1));
}
