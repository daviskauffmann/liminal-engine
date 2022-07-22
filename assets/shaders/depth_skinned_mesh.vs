#version 460 core

#include "glsl/skinned_mesh_constants.glsl"

layout (location = 0) in vec3 position;
layout (location = 5) in uint bone_ids[NUM_BONES_PER_VERTEX];
layout (location = 6) in float bone_weights[NUM_BONES_PER_VERTEX];

uniform mat4 mvp_matrix;

uniform mat4 bone_transformations[MAX_BONE_TRANSFORMATIONS];

void main()
{
    mat4 bone_transformation = mat4(0);
    for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        bone_transformation += bone_transformations[bone_ids[i]] * bone_weights[i];
    }

    gl_Position = mvp_matrix * bone_transformation * vec4(position, 1);
}
