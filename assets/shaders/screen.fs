#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

out vec4 frag_color;

uniform sampler2D hdr_map;
uniform sampler2D bloom_map;

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

	// grayscale
	// float average_color = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	// color = vec3(average_color);

	// red/green colorblindness
	// float average_color = 0.2126 * color.r + 0.7152 * color.g;
	// color = vec3(average_color, average_color, color.b);

    frag_color = vec4(color, 1.0);
}
