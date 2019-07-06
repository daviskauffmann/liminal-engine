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
	sampler2D reflection;
	sampler2D refraction;
	sampler2D dudv;
} water;

out vec4 frag_color;

void main()
{
	// distortion
	float move_factor = float(elapsed_time) / 1000 * 0.03;
	float wave_strength = 0.02;
	vec2 distortion1 = (texture(water.dudv, vec2(vertex.uv.x + move_factor, vertex.uv.y)).rg * 2.0 - 1.0) * wave_strength;
	vec2 distortion2 = (texture(water.dudv, vec2(-vertex.uv.x + move_factor, vertex.uv.y + move_factor)).rg * 2.0 - 1.0) * wave_strength;
	vec2 distortion = distortion1 + distortion2;

	vec2 proj_coords = (vertex.clip_space_position.xy / vertex.clip_space_position.w) * 0.5 + 0.5;

	// reflection
	vec2 reflection_uv = vec2(proj_coords.x, -proj_coords.y);
	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec4 reflection_color = texture(water.reflection, reflection_uv);

	// refraction
	vec2 refraction_uv = vec2(proj_coords.x, proj_coords.y);
	refraction_uv += distortion;
	refraction_uv = clamp(refraction_uv, 0.001, 0.999);
	vec4 refraction_color = texture(water.refraction, refraction_uv);
	
	// fresnel
	vec3 view_direction = normalize(camera.position - vertex.position);
	view_direction.x = abs(view_direction.x);
	view_direction.y = abs(view_direction.y);
	view_direction.z = abs(view_direction.z);
	vec3 normal = normalize(vertex.normal);
	float refractive_factor = dot(view_direction, normal);
	float reflectivity = 0.5;
	refractive_factor = pow(refractive_factor, reflectivity);

	vec4 water_color = mix(reflection_color, refraction_color, refractive_factor);

	vec4 blue_color = vec4(0.0, 0.3, 0.5, 1.0);
	float blue_factor = 0.2;

	frag_color = mix(water_color, blue_color, blue_factor);
}
