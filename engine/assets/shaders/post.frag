#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Screen {
    sampler2D texture;
} screen;

out vec4 frag_color;

void main()
{
    frag_color = texture(screen.texture, vertex.uv);
}
