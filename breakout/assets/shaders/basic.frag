#version 330 core

in struct Vertex
{
	vec2 uv;
} vertex;

uniform struct Sprite
{
	vec3 color;
	sampler2D image;
} sprite;

void main()
{    
    gl_FragColor = vec4(sprite.color, 1.0) * texture(sprite.image, vertex.uv);
}
