#version 460 core

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = projection * view * object.model * vec4(position, 1.0);
}
