#version 330 core

in struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
} vertex;

uniform vec3 ambient;

uniform struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    vec3 color;
    float shininess;
    float glow;
} material;

void main()
{
    vec3 color;

    // ambient
    color += ambient * vec3(texture(material.diffuse, vertex.uv)) * material.color;

    // emission
    color += vec3(texture(material.emission, vertex.uv)) * material.glow;

    gl_FragColor = vec4(color, 1.0);
}
