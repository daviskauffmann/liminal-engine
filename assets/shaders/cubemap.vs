#version 460 core

layout (location = 0) in vec3 position;

out struct Vertex
{
	vec3 position;
} vertex;

uniform struct Capture
{
	mat4 projection;
	mat4 view;
} capture;

void main()
{
    gl_Position = capture.projection * capture.view * vec4(position, 1.0);

    vertex.position = position;
}
