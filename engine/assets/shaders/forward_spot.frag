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
    sampler2D diffuse;
    sampler2D specular;
	float shininess;
    sampler2D normal;
    sampler2D emission;
	float glow;
} material;

uniform struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec3 attenuation;  
    float inner_cutoff;
    float outer_cutoff;
} spot_light;

out vec4 frag_color;

void main()
{
	vec3 position = vertex.position;
	vec3 diffuse = texture(material.diffuse, vertex.uv).rgb * material.color;
	vec3 normal = normalize(vertex.normal);
	vec3 specular = texture(material.specular, vertex.uv).rgb;
	float shininess = material.shininess;
	vec3 emission = texture(material.emission, vertex.uv).rgb;
	float glow = material.glow;
    vec3 view_direction = normalize(camera.position - position);
	
    // ambient
    vec3 final_ambient = spot_light.ambient * diffuse;

    // diffuse
    vec3 light_direction = normalize(spot_light.position - position);
    float diffuse_factor = max(dot(normal, light_direction), 0.0);
    vec3 final_diffuse = spot_light.diffuse * diffuse * diffuse_factor;

    // specular
    vec3 halfway_direction = normalize(light_direction + view_direction);
	float specular_angle = max(dot(normal, halfway_direction), 0.0);
    float specular_factor = pow(specular_angle, shininess);
    vec3 final_specular = spot_light.specular * specular * specular_factor;

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

    frag_color = vec4((final_ambient + (final_diffuse + final_specular) * intensity) * attenuation, 1.0);
}
