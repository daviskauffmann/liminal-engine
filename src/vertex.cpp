#include "vertex.hpp"

#include <iostream>

void liminal::vertex::add_bone_data(unsigned int id, float weight)
{
    for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        if (bone_weights[i] == 0)
        {
            bone_ids[i] = id;
            bone_weights[i] = weight;
            return;
        }
    }

    std::cerr << "Error: Trying to add more bones than the maximum" << std::endl;
}
