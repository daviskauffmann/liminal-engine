#version 460 core

layout (location = 0) in vec3 position;

uniform struct DirectionalLight
{
    mat4 projection;
    mat4 view;
} directional_light;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = directional_light.projection * directional_light.view * object.model * vec4(position, 1.0);
}
