#version 460 core

uniform vec3 color;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

void main()
{
    frag_color = vec4(1.0);
    bright_color = vec4(color, 1.0);
}
