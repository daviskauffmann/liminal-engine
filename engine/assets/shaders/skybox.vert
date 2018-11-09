#version 460 core

layout (location = 0) in vec3 position;

uniform struct Camera
{
	mat4 projection;
	mat4 view;
} camera;

out struct Vertex
{
	vec3 texture_coords;
} vertex;

void main()
{
    gl_Position = (camera.projection * camera.view * vec4(position, 1.0)).xyww;
    vertex.texture_coords = position;
}
