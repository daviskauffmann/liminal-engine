#version 460 core

layout (location = 0) in vec2 position;

out struct Vertex
{
    vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 clip_space_position;
} vertex;

uniform mat4 mvp;

uniform mat4 model;

const float tiling = 4.0;

void main()
{
    gl_Position = mvp * vec4(position.x, 0.0, position.y, 1.0);

    vertex.position = (model * vec4(position.x, 0.0, position.y, 1.0)).xyz;
	vertex.normal = vec3(0.0, 1.0, 0.0);
	vertex.uv = vec2(position.x * 0.5 + 0.5, position.y * 0.5 + 0.5) * tiling;
	vertex.clip_space_position = gl_Position;
}
