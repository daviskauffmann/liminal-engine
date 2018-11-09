#version 460 core

in struct Vertex
{
	vec2 uv;
} vertex;

uniform struct Sprite
{
	vec3 color;
	sampler2D image;
	mat4 model;
} sprite;

out vec4 frag_color;

void main()
{    
    frag_color = vec4(sprite.color, 1.0) * texture(sprite.image, vertex.uv);
}
