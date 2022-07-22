#version 460 core

#include "glsl/skinned_mesh_constants.glsl"

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in uint bone_ids[NUM_BONES_PER_VERTEX];
layout (location = 6) in float bone_weights[NUM_BONES_PER_VERTEX];

out struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform mat4 mvp_matrix;

uniform mat4 bone_transformations[MAX_BONE_TRANSFORMATIONS];

uniform mat4 model_matrix;
uniform vec4 clipping_plane;

uniform float tiling = 1;

void main()
{
    mat4 bone_transformation = mat4(0);
    for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        bone_transformation += bone_transformations[bone_ids[i]] * bone_weights[i];
    }

    vertex.position = (model_matrix * bone_transformation * vec4(position, 1)).xyz;
    vertex.normal = (model_matrix * bone_transformation * vec4(normal, 0)).xyz;
    vertex.uv = uv * tiling;

    gl_Position = mvp_matrix * bone_transformation * vec4(position, 1);
	gl_ClipDistance[0] = dot(vec4(vertex.position, 1), clipping_plane);
}
