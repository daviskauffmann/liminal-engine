#version 460 core

layout (location = 0) in vec3 position;

uniform struct Sun
{
    mat4 projection;
    mat4 view;
} sun;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = sun.projection * sun.view * object.model * vec4(position, 1.0);
}
