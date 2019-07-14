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

uniform struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
	vec3 attenuation;  
    float inner_cutoff;
    float outer_cutoff;
} spot_light;

out vec4 frag_color;

void main()
{
    // ambient
    vec3 diffuse_color = texture(geometry.albedo_specular_map, vertex.uv).rgb;
    vec3 final_ambient_color = spot_light.ambient_color * diffuse_color;

    // diffuse
    vec3 position = texture(geometry.position_map, vertex.uv).rgb;
    vec3 light_direction = normalize(spot_light.position - position);
    vec3 normal = texture(geometry.normal_map, vertex.uv).rgb;
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse_color = spot_light.diffuse_color * diffuse_color * diffuse_factor;

    // specular
    vec3 view_direction = normalize(camera.position - position);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
	float shininess = 16.0;
    float specular_factor = pow(specular_angle, shininess);
    float specular_color = texture(geometry.albedo_specular_map, vertex.uv).a;
    vec3 final_specular_color = spot_light.specular_color * specular_color * specular_factor;

    // attenuation
    float light_distance = length(spot_light.position - position);
	float constant = spot_light.attenuation[0];
	float linear = spot_light.attenuation[1];
	float quadratic = spot_light.attenuation[2];
    float attenuation = 1.0 / (constant + linear * light_distance + quadratic * pow(light_distance, 2));

    // intensity
    float theta = dot(light_direction, normalize(-spot_light.direction));
    float epsilon = spot_light.inner_cutoff - spot_light.outer_cutoff;
    float intensity = clamp((theta - spot_light.outer_cutoff) / epsilon, 0.0, 1.0);
	
    frag_color = vec4((final_ambient_color + (final_diffuse_color + final_specular_color) * intensity) * attenuation, 1.0);
}
