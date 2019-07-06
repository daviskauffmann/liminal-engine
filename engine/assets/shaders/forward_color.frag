#version 460 core

// TODO: add emissive texture and rename to forward_emissive.frag

in float visibility;

uniform struct Material
{
    vec3 color;
} material;

out vec4 frag_color;

void main()
{
    frag_color = vec4(material.color, 1.0);
	frag_color = mix(vec4(0.0, 0.0, 0.0, 1.0), frag_color, visibility);
}
