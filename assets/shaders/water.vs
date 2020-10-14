#version 460 core

layout (location = 0) in vec2 position;

out struct Vertex
{
    vec3 position;
	vec3 normal;
	vec2 uv;
	vec4 clip_space_position;
} vertex;

uniform struct Camera
{
    float near_plane;
    float far_plane;
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Water
{
	mat4 model;
	sampler2D reflection_map;
	sampler2D refraction_map;
	sampler2D depth_map;
	sampler2D dudv_map;
	sampler2D normal_map;
} water;

const float tiling = 4.0;

void main()
{
	vec4 world_position = water.model * vec4(position.x, 0.0, position.y, 1.0);

    gl_Position = camera.projection * camera.view * world_position;

    vertex.position = world_position.xyz;
	vertex.normal = mat3(transpose(inverse(water.model))) * vec3(0.0, 1.0, 0.0);
	vertex.uv = vec2(position.x * 0.5 + 0.5, position.y * 0.5 + 0.5) * tiling;
	vertex.clip_space_position = gl_Position;
}
