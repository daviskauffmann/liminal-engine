#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
} camera;

uniform struct Water
{
	mat4 model;
	sampler2D reflection;
	sampler2D refraction;
	sampler2D dudv;
} water;

out struct Vertex
{
	vec2 uv;
	vec4 clip_space;
} vertex;

void main()
{
    gl_Position = camera.projection * camera.view * water.model * vec4(position.x, 0.0, position.y, 1.0);
	vertex.uv = uv * 6.0;
	vertex.clip_space = gl_Position;
}
