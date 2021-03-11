#version 460 core

#define NUM_BONES_PER_VERTEX 10
#define MAX_BONE_TRANSFORMATIONS 100

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

uniform mat4 mvp;

uniform mat4 bone_transformations[MAX_BONE_TRANSFORMATIONS];

uniform mat4 model;
uniform vec4 clipping_plane;

uniform float tiling = 1.0;

void main()
{
    mat4 bone_transformation = mat4(0.0);
    for (int i = 0; i < NUM_BONES_PER_VERTEX; i++)
    {
        bone_transformation += bone_transformations[bone_ids[i]] * bone_weights[i];
    }

    vec4 world_position = model * bone_transformation * vec4(position, 1.0);

    gl_Position = mvp * bone_transformation * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(world_position, clipping_plane);

    vertex.position = world_position.xyz;
    vertex.normal = (model * bone_transformation * vec4(normal, 0.0)).xyz;
    vertex.uv = uv * tiling;
}
