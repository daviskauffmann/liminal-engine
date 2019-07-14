#version 460 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Material
{
    vec3 color;
    sampler2D diffuse_map;
    sampler2D specular_map;
	float shininess;
    sampler2D normal_map;
    sampler2D emission_map;
	float glow;
} material;

uniform struct Sun
{
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
	mat4 projection;
	mat4 view;
	sampler2D depth_map;
} sun;

out vec4 frag_color;

void main()
{
    // ambient
	vec3 diffuse_color = texture(material.diffuse_map, vertex.uv).rgb * material.color;
    vec3 final_ambient_color = sun.ambient_color * diffuse_color;

    // diffuse
	vec3 normal = normalize(vertex.normal);
    vec3 light_direction = normalize(-sun.direction);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse_color = sun.diffuse_color * diffuse_color * diffuse_factor;

    // specular
    vec3 view_direction = normalize(camera.position - vertex.position);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
	float shininess = material.shininess;
    float specular_factor = pow(specular_angle, shininess);
	vec3 specular_color = texture(material.specular_map, vertex.uv).rgb;
    vec3 final_specular_color = sun.specular_color * specular_color * specular_factor;

    // shadow
    vec4 sun_space_position = sun.projection * sun.view * vec4(vertex.position, 1.0);
    vec3 sun_space_proj_coords = (sun_space_position.xyz / sun_space_position.w) * 0.5 + 0.5;
    float current_depth = sun_space_proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005); 
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(sun.depth_map, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(sun.depth_map, sun_space_proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (sun_space_proj_coords.z > 1.0) shadow = 0.0;

    frag_color = vec4((final_ambient_color + (1.0 - shadow) * (final_diffuse_color + final_specular_color)), 1.0);
}