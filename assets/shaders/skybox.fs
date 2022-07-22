#version 460 core

in struct Vertex
{
	vec3 position;
} vertex;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

uniform struct Skybox
{
    samplerCube environment_cubemap;
} skybox;

void main()
{    
    vec3 color = texture(skybox.environment_cubemap, vertex.position).rgb;

	frag_color = vec4(color, 1);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 0.75)
    {
        bright_color = vec4(color, 1);
    }
    else
    {
        bright_color = vec4(0, 0, 0, 1);
    }
}
