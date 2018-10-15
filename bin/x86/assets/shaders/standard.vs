#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

void main()
{
    gl_Position = vec4(0.25 * position, 1.0);
}
