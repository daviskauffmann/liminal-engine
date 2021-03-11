#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform mat4 mvp;

uniform mat4 model;
uniform vec4 clipping_plane;

uniform float tiling = 1.0;

void main()
{
    vec4 world_position = model * vec4(position, 1.0);

    gl_Position = mvp * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(world_position, clipping_plane);

    vertex.position = world_position.xyz;
    vertex.normal = mat3(transpose(inverse(model))) * normal;
    vertex.uv = vec2(uv.x, uv.y) * tiling;
}
