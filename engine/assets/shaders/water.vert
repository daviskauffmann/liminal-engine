#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Water
{
	mat4 model;
	sampler2D reflection;
	sampler2D refraction;
	sampler2D dudv;
	sampler2D normal;
} water;

out struct Vertex
{
    vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 clip_space_position;
} vertex;

void main()
{
	vec4 world_position = water.model * vec4(position.x, 0.0, position.y, 1.0);

    gl_Position = camera.projection * camera.view * world_position;

    vertex.position = world_position.xyz;
	vertex.normal = mat3(transpose(inverse(water.model))) * vec3(0.0, 1.0, 0.0);
	vertex.uv = vec2(uv.x, 1 - uv.y) * 6.0;
	vertex.clip_space_position = gl_Position;
}
