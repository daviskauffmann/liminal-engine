#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in vec3 color;

out struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec3 color;
} vertex;

uniform mat4 mvp_matrix;

uniform mat4 model_matrix;
uniform vec4 clipping_plane;

uniform float tiling = 1;

void main()
{
    vertex.position = (model_matrix * vec4(position, 1)).xyz;
    vertex.normal = (model_matrix * vec4(normal, 0)).xyz;
    vertex.uv = uv * tiling;
    vertex.color = color;

    gl_Position = mvp_matrix * vec4(position, 1);
	gl_ClipDistance[0] = dot(vec4(vertex.position, 1), clipping_plane);
}
