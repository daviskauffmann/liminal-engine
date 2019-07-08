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
	sampler2D reflectivity_map;
	float reflectivity;
} material;

uniform struct Skybox
{
	samplerCube color_map;
} skybox;

out vec4 frag_color;

void main()
{    
    vec3 view_direction = normalize(vertex.position - camera.position);
	
	vec3 reflection = reflect(view_direction, normalize(vertex.normal));
	
	float ratio = 1.00 / 1.33;
	vec3 refraction = refract(view_direction, normalize(vertex.normal), ratio);

    frag_color = vec4((texture(skybox.color_map, reflection) * texture(material.reflectivity_map, vertex.uv) * material.reflectivity).rgb, 1.0);
}
