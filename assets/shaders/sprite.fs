#version 460 core

in struct Vertex
{
	vec2 uv;
} vertex;

out vec4 frag_color;

uniform struct Sprite
{
	sampler2D texture;
	vec3 color;
} sprite;

void main()
{    
	vec4 color = texture(sprite.texture, vertex.uv) * vec4(sprite.color, 1);
	if (color.a < 0.1)
	{
		discard;
	}

    frag_color = color;
}
