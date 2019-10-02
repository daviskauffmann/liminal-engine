#version 460 core

layout (location = 0) in vec3 position;

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

void main()
{
	vec4 world_position = object.model * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(world_position, clipping_plane);
    gl_Position = camera.projection * camera.view * world_position;
}
