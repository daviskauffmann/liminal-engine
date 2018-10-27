#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform struct Camera
{
	mat4 projection;
} camera;

uniform struct Object
{
	mat4 model;
} object;

out struct Vertex
{
	vec2 uv;
} vertex;

void main()
{
    gl_Position = camera.projection * object.model * vec4(position, 0.0, 1.0);
    vertex.uv = uv;
}
