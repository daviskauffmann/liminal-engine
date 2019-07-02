#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Water
{
	mat4 model;
	sampler2D reflection;
	sampler2D refraction;
} water;

out vec4 frag_color;

void main()
{    
	vec4 reflection_color = texture(water.reflection, vertex.uv);
	vec4 refraction_color = texture(water.refraction, vertex.uv);

	frag_color = mix(reflection_color, refraction_color, 0.5);
}
