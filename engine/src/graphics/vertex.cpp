#include <liminal/graphics/vertex.hpp>

#include <iostream>

void liminal::vertex::add_bone_data(unsigned int id, float weight)
{
    for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        if (bone_ids[i] == NUM_BONES_PER_VERTEX)
        {
            bone_ids[i] = id;
            bone_weights[i] = weight;
            return;
        }
    }
}
