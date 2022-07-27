#version 460 core

#include "glsl/cascaded_shadowmap_constants.glsl"

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out struct Vertex
{
    vec2 uv;
} vertex;

void main()
{    
    vertex.uv = uv;

    gl_Position = vec4(position, 0, 1);
}
