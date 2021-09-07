#version 460 core

layout (location = 0) in vec3 position;

uniform mat4 mvp_matrix;

uniform mat4 model_matrix;
uniform vec4 clipping_plane;

void main()
{
    gl_Position = mvp_matrix * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(model_matrix * vec4(position, 1.0), clipping_plane);
}
