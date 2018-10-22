#version 330 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Screen {
    sampler2D texture;
} screen;

void main()
{
    gl_FragColor = texture(screen.texture, vertex.uv);
}
