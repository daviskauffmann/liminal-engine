#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out struct Vertex
{
    vec2 uv;
} vertex;

void main()
{
    vertex.uv = uv;

    gl_Position = vec4(position, 1);
}
