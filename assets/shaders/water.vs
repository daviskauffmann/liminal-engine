#version 460 core

layout (location = 0) in vec2 position;

out struct Vertex
{
    vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 clip_space_position;
} vertex;

uniform mat4 mvp_matrix;

uniform mat4 model_matrix;

uniform float tiling = 1;

void main()
{
    vertex.position = (model_matrix * vec4(position.x, 0, position.y, 1)).xyz;
	vertex.normal = vec3(0, 1, 0);
	vertex.uv = vec2(position.x * 0.5 + 0.5, position.y * 0.5 + 0.5) * tiling;
	vertex.clip_space_position = mvp_matrix * vec4(position.x, 0, position.y, 1);

    gl_Position = vertex.clip_space_position;
}
