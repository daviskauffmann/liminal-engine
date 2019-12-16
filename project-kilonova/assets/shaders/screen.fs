#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Screen {
    sampler2D texture;
} screen;

out vec4 frag_color;

const float gamma = 2.2;
const float exposure = 1.0;
const float white = 1.0;

void main()
{
	vec3 color = texture(screen.texture, vertex.uv).rgb * exposure;
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mapped_luminance = (luminance * (1.0 + luminance / (white * white))) / (1.0 + luminance);
    color = (mapped_luminance / luminance) * color;
    color = pow(color, vec3(1.0 / gamma));

	// // grayscale
	// float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	// vec3 grayscale_color = vec3(average);

    frag_color = vec4(color, 1.0);
}
