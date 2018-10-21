#version 330 core

layout (location = 0) in vec3 position;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = camera.projection * camera.view * object.model * vec4(position, 1.0);
}
