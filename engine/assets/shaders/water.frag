#version 460 core

uniform struct Water
{
	mat4 model;
} water;

out vec4 frag_color;

void main()
{    
	frag_color = vec4(0.0, 0.0, 1.0, 1.0);
}
