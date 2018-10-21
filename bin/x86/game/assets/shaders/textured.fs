#version 330 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Geometry
{
    sampler2D albedospecular;
} geometry;

void main()
{
    vec3 diffuse = texture(geometry.albedospecular, vertex.uv).rgb;

    vec3 color;

    color += diffuse;

    gl_FragColor = vec4(color, 1.0);
}
