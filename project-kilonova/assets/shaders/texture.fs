#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform sampler2D color_map;
uniform vec3 color;

out vec4 frag_color;

void main()
{
	frag_color = texture(color_map, vertex.uv) * vec4(color, 1.0);
}
