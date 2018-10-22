#version 330 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D albedo;
} geometry;

void main()
{
    gl_FragColor = texture(geometry.albedo, vertex.uv);
}
