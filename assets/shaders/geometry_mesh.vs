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
    vertex.position = (model * vec4(position, 1.0)).xyz;
    vertex.normal = (model * vec4(normal, 0.0)).xyz;
    vertex.uv = uv * tiling;

    gl_Position = mvp * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(vec4(vertex.position, 1.0), clipping_plane);
}
