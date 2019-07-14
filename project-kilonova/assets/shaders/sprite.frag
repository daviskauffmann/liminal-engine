#version 460 core

in struct Vertex
{
	vec2 uv;
} vertex;

uniform struct Sprite
{
	vec3 color;
	sampler2D color_map;
	mat4 model;
} sprite;

out vec4 frag_color;

void main()
{    
	vec4 color = texture(sprite.color_map, vertex.uv);

	if (color.a < 0.1)
	{
		discard;
	}

    frag_color = vec4(sprite.color, 1.0) * color;
}
