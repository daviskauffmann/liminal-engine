#version 460 core

layout (location = 0) in vec3 position;

uniform struct Capture
{
	mat4 projection;
	mat4 view;
} capture;

out struct Vertex
{
	vec3 position;
} vertex;

void main()
{
    gl_Position = capture.projection * capture.view * vec4(position, 1.0);

    vertex.position = position;
}
