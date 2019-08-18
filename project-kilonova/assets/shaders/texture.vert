#version 460 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 uv;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
} camera;

uniform struct Object
{
    mat4 model;
} object;

uniform vec4 clipping_plane;

out struct Vertex
{
    vec2 uv;
} vertex;

void main()
{
	vec4 world_position = object.model * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(world_position, clipping_plane);
    gl_Position = camera.projection * camera.view * world_position;

    vertex.uv = vec2(uv.x, 1 - uv.y);
}
