#include <liminal/graphics/vertex.hpp>

#include <iostream>

void liminal::vertex::add_bone_data(const unsigned int id, const float weight)
{
    for (std::size_t bone_index = 0; bone_index < num_bones; bone_index++)
    {
        if (bone_ids.at(bone_index) == num_bones)
        {
            bone_ids.at(bone_index) = id;
            bone_weights.at(bone_index) = weight;
            return;
        }
    }
}
