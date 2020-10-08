#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

out vec4 frag_color;

uniform sampler2D hdr_map;
uniform sampler2D bloom_map;

uniform uint greyscale;

const float gamma = 2.2;
const float exposure = 1.0;
const float white = 1.0;

void main()
{
	vec3 hdr_color = texture(hdr_map, vertex.uv).rgb;
	vec3 bloom_color = texture(bloom_map, vertex.uv).rgb;

	vec3 color = (hdr_color + bloom_color) * exposure;
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mapped_luminance = (luminance * (1.0 + luminance / (white * white))) / (1.0 + luminance);
    color = (mapped_luminance / luminance) * color;
    color = pow(color, vec3(1.0 / gamma));

	if (greyscale == 1)
	{
		color = vec3((color.r + color.g + color.b) / 3);
	}

    frag_color = vec4(color, 1.0);
}
