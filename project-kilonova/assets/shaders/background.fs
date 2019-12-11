#version 460 core

in struct Vertex
{
	vec3 position;
} vertex;

uniform samplerCube environment_cubemap;

out vec4 frag_color;

void main()
{    
    vec3 environment_color = texture(environment_cubemap, vertex.position).rgb;
	environment_color = environment_color / (environment_color + vec3(1.0));
	environment_color = pow(environment_color, vec3(1.0 / 2.2));

	frag_color = vec4(environment_color, 1.0);
}
