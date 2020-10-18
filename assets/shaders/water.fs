#version 460 core

in struct Vertex
{
    vec3 position;
	vec3 normal;
    vec2 uv;
	vec4 clip_space_position;
} vertex;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

uniform struct Camera
{
    float near_plane;
    float far_plane;
    vec3 position;
} camera;

uniform struct Water
{
	sampler2D reflection_map;
	sampler2D refraction_map;
	sampler2D depth_map;
	sampler2D dudv_map;
	sampler2D normal_map;
} water;

uniform struct Light
{
    vec3 direction;
    vec3 color;
} light;

uniform uint current_time;

const float speed = 0.02;
const float wave_strength = 0.01;
const float reflectivity = 0.5;
const float shine_damper = 100.0;

void main()
{
	vec2 clip_space_proj_coords = (vertex.clip_space_position.xy / vertex.clip_space_position.w) * 0.5 + 0.5;
	vec2 reflection_uv = vec2(clip_space_proj_coords.x, -clip_space_proj_coords.y);
	vec2 refraction_uv = vec2(clip_space_proj_coords.x, clip_space_proj_coords.y);

	float depth_to_floor = texture(water.depth_map, refraction_uv).r;
	float distance_to_floor = 2.0 * camera.near_plane * camera.far_plane / (camera.far_plane + camera.near_plane - (2.0 * depth_to_floor - 1.0) * (camera.far_plane - camera.near_plane));
	float depth_to_surface = gl_FragCoord.z;
	float distance_to_surface = 2.0 * camera.near_plane * camera.far_plane / (camera.far_plane + camera.near_plane - (2.0 * depth_to_surface - 1.0) * (camera.far_plane - camera.near_plane));
	float water_depth = distance_to_floor - distance_to_surface;

	float move_factor = float(current_time) / 1000 * speed;
	vec2 distorted_uv = texture(water.dudv_map, vec2(vertex.uv.x + move_factor, vertex.uv.y)).rg * 0.1;
	distorted_uv += vertex.uv + vec2(distorted_uv.x, distorted_uv.y + move_factor);
	vec2 distortion = (texture(water.dudv_map, distorted_uv).rg * 2.0 - 1.0) * wave_strength * clamp(water_depth / 20.0, 0.0, 1.0);

	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec3 reflection_color = texture(water.reflection_map, reflection_uv).rgb;

	refraction_uv += distortion;
	refraction_uv = clamp(refraction_uv, 0.001, 0.999);
	vec3 refraction_color = texture(water.refraction_map, refraction_uv).rgb;

	vec4 normal_color = texture(water.normal_map, distorted_uv);
	vec3 normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b * 3.0, normal_color.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	vec3 view_direction = normalize(camera.position - vertex.position);
	float refractive_factor = dot(abs(view_direction), normal);
	refractive_factor = pow(refractive_factor, reflectivity);
	refractive_factor = clamp(refractive_factor, 0.0, 1.0);

	vec3 light_reflection = reflect(normalize(light.direction), normal);
	float specular_factor = pow(max(dot(light_reflection, view_direction), 0.0), shine_damper);
	vec3 specular = light.color * specular_factor * reflectivity * clamp(water_depth / 5.0, 0.0, 1.0);

	vec3 color = mix(reflection_color, refraction_color, refractive_factor) + specular;

	frag_color = vec4(color, clamp(water_depth / 5.0, 0.0, 1.0));

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
    {
        bright_color = vec4(color, 1.0);
    }
    else
    {
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
