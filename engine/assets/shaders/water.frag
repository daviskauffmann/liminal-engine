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
	sampler2D normal;
} water;

uniform struct Sun
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	mat4 projection;
	mat4 view;
} sun;

uniform struct Depthmap {
    sampler2D texture;
} depthmap;

out vec4 frag_color;

void main()
{
	// distortion
	float move_factor = float(elapsed_time) / 1000 * 0.03;
	float wave_strength = 0.02;
	vec2 distorted_uv = texture(water.dudv, vec2(vertex.uv.x + move_factor, vertex.uv.y)).rg * 0.1;
	distorted_uv += vertex.uv + vec2(distorted_uv.x, distorted_uv.y + move_factor);
	vec2 distortion = (texture(water.dudv, distorted_uv).rg * 2.0 - 1.0) * wave_strength;

	vec2 clip_space_proj_coords = (vertex.clip_space_position.xy / vertex.clip_space_position.w) * 0.5 + 0.5;

	// reflection
	vec2 reflection_uv = vec2(clip_space_proj_coords.x, -clip_space_proj_coords.y);
	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec4 reflection_color = texture(water.reflection, reflection_uv);

	// refraction
	vec2 refraction_uv = vec2(clip_space_proj_coords.x, clip_space_proj_coords.y);
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

	// normal
	vec4 normal_color = texture(water.normal, distorted_uv);
	vec3 modified_normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b, normal_color.g * 2.0 - 1.0);
	modified_normal = normalize(modified_normal);

	// specular
    vec3 light_direction = normalize(-sun.direction);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(modified_normal, halfway_direction), 0.0);
	float shininess = 16.0; // TODO: water shininess uniform?
    float specular_factor = pow(specular_angle, shininess) * 0.6;
	vec3 specular = vec3(1.0, 1.0, 1.0);
    vec3 final_specular = sun.specular * specular * specular_factor;

    // shadow
    vec4 sun_space_position = sun.projection * sun.view * vec4(vertex.position, 1.0);
    vec3 sun_space_proj_coords = (sun_space_position.xyz / sun_space_position.w) * 0.5 + 0.5;
    float current_depth = sun_space_proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005); 
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(depthmap.texture, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(depthmap.texture, sun_space_proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (sun_space_proj_coords.z > 1.0) shadow = 0.0;

	// final water color
	vec4 water_color = mix(reflection_color, refraction_color, refractive_factor) + (1 - shadow) * vec4(final_specular, 0.0);

	// blue tint
	vec4 blue_color = vec4(0.0, 0.3, 0.5, 1.0);
	float blue_factor = 0.2;

	frag_color = mix(water_color, blue_color, blue_factor);
}
