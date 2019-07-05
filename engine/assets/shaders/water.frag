#version 460 core

in struct Vertex
{
    vec2 uv;
	vec4 clip_space;
} vertex;

uniform struct Water
{
	mat4 model;
	sampler2D reflection;
	sampler2D refraction;
	sampler2D dudv;
} water;

out vec4 frag_color;

void main()
{
	vec2 ndc = (vertex.clip_space.xy / vertex.clip_space.w) / 2.0 + 0.5;
	vec2 distortion = (texture(water.dudv, vertex.uv).rg * 2.0 - 1.0) * 0.02;

	vec2 reflection_uv = vec2(ndc.x, -ndc.y);
	reflection_uv += distortion;
	reflection_uv.x = clamp(reflection_uv.x, 0.001, 0.999);
	reflection_uv.y = clamp(reflection_uv.y, -0.999, -0.001);
	vec4 reflection_color = texture(water.reflection, reflection_uv);

	vec2 refraction_uv = vec2(ndc.x, ndc.y);
	refraction_uv += distortion;
	refraction_uv = clamp(refraction_uv, 0.001, 0.999);
	vec4 refraction_color = texture(water.refraction, refraction_uv);

	frag_color = mix(reflection_color, refraction_color, 0.5);
}
