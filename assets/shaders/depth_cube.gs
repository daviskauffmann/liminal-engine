#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 frag_position;

uniform struct Light
{
    float far_plane;
    vec3 position;
    mat4 view_projection_matrices[6];
} light;

void main()
{
    for (int face = 0; face < 6; face++)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; i++)
        {
            frag_position = gl_in[i].gl_Position;
            gl_Position = light.view_projection_matrices[face] * frag_position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
