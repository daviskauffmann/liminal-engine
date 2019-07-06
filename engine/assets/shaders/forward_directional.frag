#version 460 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

in float visibility;

uniform struct Camera
{
    mat4 projection;
    mat4 view;
    vec3 position;
} camera;

uniform struct Material
{
    vec3 color;
    sampler2D diffuse;
    sampler2D specular;
	float shininess;
    sampler2D normal;
    sampler2D emission;
	float glow;
} material;

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
    // ambient
	vec3 diffuse = texture(material.diffuse, vertex.uv).rgb * material.color;
    vec3 final_ambient = directional_light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(-directional_light.direction);
	vec3 normal = normalize(vertex.normal);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = directional_light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 view_direction = normalize(camera.position - vertex.position);
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
	float shininess = material.shininess;
    float specular_factor = pow(specular_angle, shininess);
	vec3 specular = texture(material.specular, vertex.uv).rgb;
    vec3 final_specular = directional_light.specular * specular * specular_factor;

    frag_color = vec4(final_ambient + final_diffuse + final_specular, 1.0);
	frag_color = mix(vec4(0.0, 0.0, 0.0, 1.0), frag_color, visibility);
}
