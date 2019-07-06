#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

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

uniform vec4 clipping_plane;

out struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

out float visibility;

void main()
{
	vec4 world_position = object.model * vec4(position, 1.0);

	gl_ClipDistance[0] = dot(world_position, clipping_plane);
    gl_Position = camera.projection * camera.view * world_position;

    vertex.position = world_position.xyz;
    vertex.normal = mat3(transpose(inverse(object.model))) * normal;
    vertex.uv = vec2(uv.x, 1 - uv.y);

	float dist = length(gl_Position.xyz);
	float density = 0.007;
	float gradient = 1.5;
	visibility = exp(-pow(dist * density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}
