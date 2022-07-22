#version 460 core

layout (location = 0) in vec3 position;

out struct Vertex
{
	vec3 position;
} vertex;

uniform mat4 mvp_matrix;

void main()
{
    vertex.position = position;

    gl_Position = mvp_matrix * vec4(position, 1);
}
