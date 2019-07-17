#version 460 core

in struct Vertex
{
    vec2 uv;
} vertex;

uniform struct Screen {
    sampler2D color_map;
} screen;

out vec4 frag_color;

void main()
{
	vec4 color = texture(screen.color_map, vertex.uv);

	// grayscale
	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	vec3 grayscale_color = vec3(average);
    vec4 final_grayscale_color = vec4(grayscale_color, 1.0);

	const float offset = 1.0 / 600.0;  
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),  
        vec2( 0.0f,    0.0f),  
        vec2( offset,  0.0f),  
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)    
    );
    vec3 sample_texture[9];
    for(int i = 0; i < 9; i++)
    {
        sample_texture[i] = vec3(texture(screen.color_map, vertex.uv + offsets[i]));
    }

	// sharpen
	float sharpen_kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);
    vec3 sharpen_color = vec3(0.0);
    for(int i = 0; i < 9; i++)
	{
        sharpen_color += sample_texture[i] * sharpen_kernel[i];
	}
	vec4 final_sharpen_color = vec4(sharpen_color, 1.0);

	// blur
	float blur_kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16  
	);
    vec3 blur_color = vec3(0.0);
    for(int i = 0; i < 9; i++)
	{
        blur_color += sample_texture[i] * blur_kernel[i];
	}
	vec4 final_blur_color = vec4(blur_color, 1.0);

	frag_color = color;
}
