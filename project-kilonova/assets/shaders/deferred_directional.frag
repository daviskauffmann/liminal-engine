#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D position_map;
    sampler2D normal_map;
    sampler2D albedo_specular_map;
} geometry;

uniform struct Camera
{
    vec3 position;
} camera;

uniform struct DirectionalLight
{
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
	mat4 projection;
	mat4 view;
	sampler2D depth_map;
} directional_light;

out vec4 frag_color;

void main()
{
    // ambient
    vec3 diffuse_color = texture(geometry.albedo_specular_map, vertex.uv).rgb;
    vec3 final_ambient_color = directional_light.ambient_color * diffuse_color;

    // diffuse
    vec3 light_direction = normalize(-directional_light.direction);
    vec3 normal = texture(geometry.normal_map, vertex.uv).rgb;
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse_color = directional_light.diffuse_color * diffuse_color * diffuse_factor;

    // specular
    vec3 position = texture(geometry.position_map, vertex.uv).rgb;
    vec3 view_direction = normalize(camera.position - position);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
	float shininess = 16.0;
    float specular_factor = pow(specular_angle, shininess);
    float specular_color = texture(geometry.albedo_specular_map, vertex.uv).a;
    vec3 final_specular_color = directional_light.specular_color * specular_color * specular_factor;
	
    // shadow
    vec4 light_space_position = directional_light.projection * directional_light.view * vec4(position, 1.0);
    vec3 light_space_proj_coords = (light_space_position.xyz / light_space_position.w) * 0.5 + 0.5;
    float current_depth = light_space_proj_coords.z;
    float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005); 
    float shadow = 0.0;
    vec2 texel_size = 1.0 / textureSize(directional_light.depth_map, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcf_depth = texture(directional_light.depth_map, light_space_proj_coords.xy + vec2(x, y) * texel_size).r;
            shadow += current_depth - bias > pcf_depth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if (light_space_proj_coords.z > 1.0) shadow = 0.0;

    frag_color = vec4((final_ambient_color + (1.0 - shadow) * (final_diffuse_color + final_specular_color)), 1.0);
}
