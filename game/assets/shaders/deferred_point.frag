#version 330 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D position;
    sampler2D normal;
    sampler2D albedo_specular;
} geometry;

uniform struct Camera
{
    vec3 position;
} camera;

uniform struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 attenuation;
} point_light;

void main()
{
    vec3 position = texture(geometry.position, vertex.uv).rgb;
    vec3 normal = texture(geometry.normal, vertex.uv).rgb;
    vec3 diffuse = texture(geometry.albedo_specular, vertex.uv).rgb;
    float specular = texture(geometry.albedo_specular, vertex.uv).a;
	float shininess = 16.0;
    vec3 view_direction = normalize(camera.position - position);

	// ambient
    vec3 final_ambient = point_light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(point_light.position - position);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = point_light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
    float specular_factor = pow(specular_angle, shininess);
    vec3 final_specular = point_light.specular * specular * specular_factor;

    // attenuation
    float light_distance = length(point_light.position - position);
	float constant = point_light.attenuation[0];
	float linear = point_light.attenuation[1];
	float quadratic = point_light.attenuation[2];
    float attenuation = 1.0 / (constant + linear * light_distance + quadratic * pow(light_distance, 2));

    gl_FragColor = vec4((final_ambient + final_diffuse + final_specular) * attenuation, 1.0);
}
