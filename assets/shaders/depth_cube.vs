#version 460 core

layout (location = 0) in vec3 position;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = object.model * vec4(position, 1.0);
}
