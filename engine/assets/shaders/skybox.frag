#version 460 core

in struct Vertex
{
	vec3 uv;
} vertex;

uniform struct Skybox
{
	samplerCube cubemap;
} skybox;

out vec4 frag_color;

void main()
{    
    frag_color = texture(skybox.cubemap, vertex.uv);
}
