#version 460 core

layout (location = 0) in vec3 position;

uniform mat4 mvp;

uniform mat4 model;
uniform vec4 clipping_plane;

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
	gl_ClipDistance[0] = dot(model * vec4(position, 1.0), clipping_plane);
}
