#version 460 core

in struct Vertex
{
	vec2 uv;
} vertex;

uniform struct Sprite
{
	sampler2D texture;
	vec3 color;
	mat4 model;
} sprite;

out vec4 frag_color;

void main()
{    
	vec4 color = texture(sprite.texture, vertex.uv) * vec4(sprite.color, 1.0);
	if (color.a < 0.1)
	{
		discard;
	}

    frag_color = color;
}
