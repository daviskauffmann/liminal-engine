#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 frag_position;

uniform mat4 mvps[6];

void main()
{
    for (int face = 0; face < 6; face++)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; i++)
        {
            frag_position = gl_in[i].gl_Position;
            gl_Position = mvps[face] * frag_position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
