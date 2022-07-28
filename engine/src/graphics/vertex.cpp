#include <liminal/graphics/vertex.hpp>

#include <iostream>

void liminal::vertex::add_bone_data(const unsigned int id, const float weight)
{
    for (std::size_t i = 0; i < num_bones; i++)
    {
        if (bone_ids[i] == num_bones)
        {
            bone_ids[i] = id;
            bone_weights[i] = weight;
            return;
        }
    }
}
