#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform struct Camera
{
	mat4 projection;
} camera;

uniform struct Sprite
{
	vec3 color;
	sampler2D image;
	mat4 model;
} sprite;

out struct Vertex
{
	vec2 uv;
} vertex;

void main()
{
    gl_Position = camera.projection * sprite.model * vec4(position, 0.0, 1.0);
    vertex.uv = uv;
}
