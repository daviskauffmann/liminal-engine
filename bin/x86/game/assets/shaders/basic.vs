#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uv;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform int time;

uniform struct Object
{
    mat4 model;
} object;

out struct Vertex
{
    vec3 position;
    vec2 uv;
} vertex;

void main()
{
    gl_Position = camera.projection * camera.view * object.model * vec4(position, 1.0);
    vertex.position = vec3(object.model * vec4(position, 1.0));
    vertex.uv = vec2(uv.x, 1 - uv.y);
}
