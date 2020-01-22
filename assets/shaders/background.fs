#version 460 core

in struct Vertex
{
	vec3 position;
} vertex;

uniform samplerCube environment_cubemap;

out vec4 frag_color;

void main()
{    
    vec3 color = texture(environment_cubemap, vertex.position).rgb;

	frag_color = vec4(color, 1.0);
}
