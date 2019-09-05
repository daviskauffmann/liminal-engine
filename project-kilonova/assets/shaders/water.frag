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
	sampler2D dudv_map;
	sampler2D normal_map;
} water;

uniform struct Sun
{
    vec3 direction;
    vec3 specular_color;
} sun;

out vec4 frag_color;

void main()
{
	// distortion
	float move_factor = float(elapsed_time) / 1000 * 0.03;
	float wave_strength = 0.04;
	vec2 distorted_uv = texture(water.dudv_map, vec2(vertex.uv.x + move_factor, vertex.uv.y)).rg * 0.1;
	distorted_uv += vertex.uv + vec2(distorted_uv.x, distorted_uv.y + move_factor);
	vec2 distortion = (texture(water.dudv_map, distorted_uv).rg * 2.0 - 1.0) * wave_strength;

	vec2 clip_space_proj_coords = (vertex.clip_space_position.xy / vertex.clip_space_position.w) * 0.5 + 0.5;

	// reflection
	vec2 reflection_uv = vec2(clip_space_proj_coords.x, -clip_space_proj_coords.y);
	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec4 reflection_color = texture(water.reflection_map, reflection_uv);

	// refraction
	vec2 refraction_uv = vec2(clip_space_proj_coords.x, clip_space_proj_coords.y);
	refraction_uv += distortion;
	refraction_uv = clamp(refraction_uv, 0.001, 0.999);
	vec4 refraction_color = texture(water.refraction_map, refraction_uv);
	
	// fresnel
	vec3 view_direction = normalize(camera.position - vertex.position);
	view_direction.x = abs(view_direction.x);
	view_direction.y = abs(view_direction.y);
	view_direction.z = abs(view_direction.z);
	vec3 normal = normalize(vertex.normal);
	float refractive_factor = dot(view_direction, normal);
	float reflectivity = 0.5;
	refractive_factor = pow(refractive_factor, reflectivity);

	// normal
	vec4 normal_color = texture(water.normal_map, distorted_uv);
	vec3 modified_normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b, normal_color.g * 2.0 - 1.0);
	modified_normal = normalize(modified_normal);

	// specular
    vec3 light_direction = normalize(-sun.direction);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(modified_normal, halfway_direction), 0.0);
	float shine_damper = 20.0;
    float specular_factor = pow(specular_angle, shine_damper) * 0.6;
	vec3 specular_color = vec3(1.0, 1.0, 1.0);
    vec3 final_specular_color = sun.specular_color * specular_color * specular_factor;

	// final water color
	vec4 water_color = mix(reflection_color, refraction_color, refractive_factor); // + vec4(final_specular_color, 0.0);

	// blue tint
	vec4 blue_color = vec4(0.0, 0.3, 0.5, 1.0);
	float blue_factor = 0.2;

	frag_color = mix(water_color, blue_color, blue_factor);
}
