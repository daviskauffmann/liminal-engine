#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in uint bone_ids[4];
layout (location = 6) in float bone_weights[4];

out struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform mat4 mvp;

uniform mat4 bone_transformations[100];

uniform mat4 model;
uniform vec4 clipping_plane;

uniform float tiling;

void main()
{
    mat4 bone_transformation = bone_transformations[bone_ids[0]] * bone_weights[0];
    bone_transformation += bone_transformations[bone_ids[1]] * bone_weights[1];
    bone_transformation += bone_transformations[bone_ids[2]] * bone_weights[2];
    bone_transformation += bone_transformations[bone_ids[3]] * bone_weights[3];

    vec4 world_position = model * bone_transformation * vec4(position, 1.0);

    gl_Position = mvp * bone_transformation * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(world_position, clipping_plane);

    vertex.position = world_position.xyz;
    vertex.normal = mat3(transpose(inverse(model))) * normal;
    vertex.uv = vec2(uv.x, uv.y) * tiling;
}
