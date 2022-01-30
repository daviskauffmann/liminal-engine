#include "mesh_data.hpp"

void minecraft::mesh_data::add_quad_indices()
{
    indices.push_back((unsigned int)(vertices.size() - 4));
    indices.push_back((unsigned int)(vertices.size() - 3));
    indices.push_back((unsigned int)(vertices.size() - 2));

    indices.push_back((unsigned int)(vertices.size() - 4));
    indices.push_back((unsigned int)(vertices.size() - 2));
    indices.push_back((unsigned int)(vertices.size() - 1));
}
