#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uv;

out struct Vertex
{
    vec2 uv;
} vertex;

void main()
{
    gl_Position = vec4(position.xy, 0.0, 1.0);
    vertex.uv = uv;
}
