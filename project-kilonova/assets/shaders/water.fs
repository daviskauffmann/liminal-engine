#version 460 core

in struct Vertex
{
    vec3 position;
	vec3 normal;
    vec2 uv;
	vec4 clip_space_position;
} vertex;

uniform unsigned int elapsed_time;

uniform struct Camera
{
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

uniform float near_plane;
uniform float far_plane;

out vec4 frag_color;

const float wave_strength = 0.04;
const float reflectivity = 0.5;

void main()
{
	vec2 clip_space_proj_coords = (vertex.clip_space_position.xy / vertex.clip_space_position.w) * 0.5 + 0.5;
	vec2 reflection_uv = vec2(clip_space_proj_coords.x, -clip_space_proj_coords.y);
	vec2 refraction_uv = vec2(clip_space_proj_coords.x, clip_space_proj_coords.y);

	// float depth_to_floor = texture(water.depth_map, refraction_uv).r;
	// float distance_to_floor = 2.0 * near_plane * far_plane / (far_plane + near_plane - (2.0 * depth_to_floor - 1.0) * (far_plane - near_plane));
	// float depth_to_surface = gl_FragCoord.z;
	// float distance_to_surface = 2.0 * near_plane * far_plane / (far_plane + near_plane - (2.0 * depth_to_surface - 1.0) * (far_plane - near_plane));
	// float water_depth = distance_to_floor - distance_to_surface;

	float move_factor = float(elapsed_time) / 1000 * 0.03;
	vec2 distorted_uv = texture(water.dudv_map, vec2(vertex.uv.x + move_factor, vertex.uv.y)).rg * 0.1;
	distorted_uv += vertex.uv + vec2(distorted_uv.x, distorted_uv.y + move_factor);
	vec2 distortion = (texture(water.dudv_map, distorted_uv).rg * 2.0 - 1.0) * wave_strength; // clamp(water_depth / 20.0, 0.0, 1.0);

	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec4 reflection_color = texture(water.reflection_map, reflection_uv);

	refraction_uv += distortion;
	refraction_uv = clamp(refraction_uv, 0.001, 0.999);
	vec4 refraction_color = texture(water.refraction_map, refraction_uv);

	vec4 normal_color = texture(water.normal_map, distorted_uv);
	vec3 normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b, normal_color.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	vec3 view_direction = normalize(camera.position - vertex.position);
	view_direction.x = abs(view_direction.x);
	view_direction.y = abs(view_direction.y);
	view_direction.z = abs(view_direction.z);
	float refractive_factor = dot(view_direction, normal);
	refractive_factor = pow(refractive_factor, reflectivity);
	refractive_factor = clamp(refractive_factor, 0.0, 1.0);

	vec4 water_color = mix(reflection_color, refraction_color, refractive_factor);
	vec4 blue_color = vec4(0.0, 0.3, 0.5, 1.0);
	float blue_factor = 0.2;
	frag_color = mix(water_color, blue_color, blue_factor);
	// frag_color.a = clamp(water_depth / 5.0, 0.0, 1.0);
}
