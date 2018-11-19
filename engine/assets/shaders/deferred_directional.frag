#version 460 core

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

uniform struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} directional_light;

uniform struct Depthmap {
    sampler2D texture;
} depthmap;

out vec4 frag_color;

void main()
{
    vec3 position = texture(geometry.position, vertex.uv).rgb;
    vec3 normal = texture(geometry.normal, vertex.uv).rgb;
    vec3 diffuse = texture(geometry.albedo_specular, vertex.uv).rgb;
    float specular = texture(geometry.albedo_specular, vertex.uv).a;
	float shininess = 16.0;
    vec3 view_direction = normalize(camera.position - position);

    // ambient
    vec3 final_ambient = directional_light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(-directional_light.direction);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = directional_light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
    float specular_factor = pow(specular_angle, shininess);
    vec3 final_specular = directional_light.specular * specular * specular_factor;

    frag_color = vec4(final_ambient + final_diffuse + final_specular, 1.0);
}
