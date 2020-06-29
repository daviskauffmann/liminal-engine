#version 460 core

layout (location = 0) in vec3 position;

out struct Vertex
{
	vec3 position;
} vertex;

uniform struct Camera
{
	mat4 projection;
	mat4 view;
} camera;

void main()
{
    gl_Position = (camera.projection * camera.view * vec4(position, 1.0)).xyww;

    vertex.position = position;
}
