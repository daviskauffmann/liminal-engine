#version 330 core

layout (location = 0) in vec3 position;

uniform struct Scene
{
    mat4 sun_projection;
    mat4 sun_view;
} scene;

uniform struct Object
{
    mat4 model;
} object;

void main()
{
    gl_Position = scene.sun_projection * scene.sun_view * object.model * vec4(position, 1.0);
}
