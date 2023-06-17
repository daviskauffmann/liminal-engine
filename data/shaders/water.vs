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

uniform struct Water
{
	sampler2D reflection_map;
	sampler2D refraction_map;
	sampler2D depth_map;
	sampler2D dudv_map;
	sampler2D normal_map;

    float tiling;
    float speed;
    float wave_strength;
    float reflectivity;
    float shine_damper;
} water;

void main()
{
    vertex.position = (model_matrix * vec4(position.x, 0, position.y, 1)).xyz;
	vertex.normal = vec3(0, 1, 0);
	vertex.uv = vec2(position.x * 0.5 + 0.5, position.y * 0.5 + 0.5) * water.tiling;
	vertex.clip_space_position = mvp_matrix * vec4(position.x, 0, position.y, 1);

    gl_Position = vertex.clip_space_position;
}
